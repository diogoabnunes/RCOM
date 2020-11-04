#include "app_recetor.h"

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
    //int numDataPack = 1;

    while(1){
        lido = llread(fd,pack); //lido = tamanho lido em pack

        if(lido<0)
            printf("Erro na leitura em appRecetor()...\n");

        if (pack[0] == C_START){ //verifica se é pacote de controlo inicial
            //printf("Pacote de controlo inicial: ");
            file_content(pack, lido); //guarda o conteudo em pack

            strcat(app.destination, app.file);
            fdfile = open(app.destination, O_RDWR | O_CREAT, 0777);
            continue;
        }
        else if (pack[0] == C_DATA && lido > 0) {//verifica se é pacote de dados
            psize = pack[3] + pack[2] * 256; //(K = 256 * L2 + L1)

            if (pack[1] != counter){
                off_t offset = (pack[1] + move) * (MAX_SIZE-4);
                lseek(fdfile, offset, SEEK_SET);
            }

            //printf("Pacote de dados nº %d: ", numDataPack);
            write(fdfile,&pack[4], psize); //escreve conteudo no ficheiro
            //numDataPack++;

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

        else if (pack[0] == C_END){
            //printf("Pacote de controlo final: ");
            break;
        }
    }

    if (close(fdfile)<0){
        printf("Erro ao fechar ficheiro em appRecetor()...\n");
    }

    return fd;
}