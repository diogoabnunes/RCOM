#include "data_link.h"

struct applicationLayer {
  int type;
  char port[255];
  char filename[255];
  char destination[255];

  char file[255];
  off_t filesize;
} app;


int appEmissor(int fd, char *file){

    struct stat ficheiro; //stat ficheiro do pinguim
    if (stat(app.filename, &ficheiro)<0){
        printf("error stat appEmissor.\n");
        return -1;
    }

    int fdfile = open(app.filename,O_RDONLY); // abre o ficheiro do pinguim
    if (fdfile <0){
        printf("erro ao abrir ficheiro em appEmissor.\n");
        return -1;
    }

    char cPack[255]; //Pacotes de controlo para sinalizar o início e o fim da transferência do ficheiro
    int size = sizeof(ficheiro.st_size);
    cPack[0] = C_START; //campo de controlo para indicar o start
    cPack[1] = T_SIZE; //parametro, neste caso tamanho do ficheiro
    cPack[2] = size; //tamanho em octetos (campo V)
    memcpy(&cPack[3],&ficheiro.st_size,size); //valor

    cPack[size+3] = T_NAME; //parametro para indicar nome
    cPack[size+4] = strlen(app.filename); //tamanho do ficheiro em octetos
    memcpy(&cPack[size+5],app.filename,strlen(app.filename)); //nome do ficheiro (pinguim)

    if (llwrite(fd,cPack,size + 5 + strlen(app.filename))<0){ //envia o pacote de controlo para fd
        printf("erro ao escrever cPack em appEmissor");
        return -1;
    }

    int num = 0;
    char dPack[MAX_SIZE]; //pacote de dados
    char * file_data[MAX_SIZE];//dados do ficheiro
    int bytes_number; //número de bytes
    while((bytes_number = read(fdfile,file_data,MAX_SIZE-4)) != 0){
//guarda conteudo em file_data e numero de bytes em bytes_number


        dPack[0] = DATA; //valor 1, parametro de dados
        dPack[1] = num % 255; //numero de sequencia (modulo de 255)
        //(K = 256 * L2 + L1)
        dPack[2] = bytes_number / 256; //L2
        dPack[3] = bytes_number % 256; //L1 resto
        memcpy(&dPack[4],file_data,bytes_number); //dados

        if (llwrite(fd,dPack,bytes_number+4) != 0){
          //escreve os dados para fd
            printf("erro ao escrever dPack em appEmissor");
            return -1;
        }
        num++;
    }
    cPack[0] = END; //pacote de controlo final é igual ao inicial à excessão do parametro cPack[0]

    if (llwrite(fd,cPack,size + 5 + strlen(app.filename)) != 0){
      //escreve o pacote de controlo final para fd
        printf("erro ao escrever cPack 2 em appEmissor");
        return -1;
    }
    return 0;
}

void file_content(unsigned char *pack, int psize){
  off_t size = 0;
  int counter = 1;
  int info;

  while (counter != psize)
  {
    if (pack[counter]==T_SIZE){ //se for parametro de tamanho
      int info = pack[counter+1]; //size
      for (int i = counter+2, k=0; i < info+counter+2; i++, k++){ //dados a partir de pack[3];
        size += pack[i] << 8*k; //vai adicionando octetos mais significativos (com shift para a esquerda)
      }
      app.filesize = size;
      counter=info+3; // salta os bytes que nao são dados (start, parametro e size)
    }

    if (pack[counter]==T_NAME){ //se for parametro de nome
      info = pack[counter+1]; //size
      for (int i =counter+2, k=0; i < info+counter+2; i++, k++){//dados a partir de pack[3]
        app.file[k] = pack[i]; //adiciona dados ao ficheiro
      }
      app.file[info+counter+2] = '\0'; //indicador de fim de ficheiro
      counter += 2 + info; //salta os bytes que nao sao de dados
    }
  }
}

int appRecetor(int fd){
    unsigned char pack[MAX_SIZE];
    int fdfile, lido, psize;
    int counter=0, counter2;
    int move = 0; //offsets de 255

    while(1){
        lido = llread(fd,pack); //lido = tamanho lido em pack

        if(lido<0)
            printf("nada foi lido em appRecetor");

        if (pack[0] == C_START){ //verifica se é pacote de controlo
            file_content(pack, lido); //guarda o conteudo em pack

            strcat(app.destination, app.file);
            fdfile = open(app.destination, O_RDWR | O_CREAT, 0777);
            continue;
        }
        else if (pack[0] == DATA && lido > 0) {//verifica se é pacote de dados
            psize = pack[3] + pack[2] * 256; //(K = 256 * L2 + L1)

            if (pack[1] != counter){
                off_t offset = (pack[1] + move) * (MAX_SIZE-4);
                lseek(fdfile, offset, SEEK_SET);
            }

            write(fdfile,&pack[4], psize); //escreve conteudo no ficheiro

            if (pack[1] != counter){
                lseek(fdfile, 0 , 4); // SEEK_HOLE
            }

            if (pack[1]== counter){
                counter++;
            }

            counter2 = counter;
            counter %= 255;

            if (counter % 255 == 0 && counter2!= 0){
                move+=255;
            }
        }

        else if (pack[0] == END){
            break;
        }

    }

    if (close(fdfile)<0){
        printf("erro ao fechar ficheiro em appRecetor\n");
    }

    return fd;
}



void parseArguments(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: ./app emissor/recetor serialPort filename/destination");
        exit(1);
    }
    if (strcmp("emissor", argv[1]) == 0) {
        app.type = EMISSOR;
        strcpy(app.filename, argv[3]);
    }
    else if (strcmp("recetor", argv[1]) == 0) {
        app.type = RECETOR;
        strcpy(app.destination, argv[3]);
    }

    strcpy(app.port, argv[2]);

    if (app.type == EMISSOR) printf("Emissor\n\n");
    else if (app.type == RECETOR) printf("Recetor\n\n");
}

int main(int argc, char** argv) {
    printf("\n\n\nAplicação - RCOM - TL1\n");

    parseArguments(argc, argv);

    int fd = llopen(app.port, app.type);
    if (fd < 0) {
        printf("Erro em llopen()...\n");
        exit(2);
    }

    if (app.type == EMISSOR) appEmissor(fd, app.filename);
    else if (app.type == RECETOR) appRecetor(fd);

    if (llclose(fd) < 0) {
        printf("Erro em llclose()\n");
        exit(3);
    }

    if (app.type == EMISSOR) printf("Emissor terminou execução.");
    else if (app.type == RECETOR) printf("Recetor terminou execução.");

    return 0;
}
