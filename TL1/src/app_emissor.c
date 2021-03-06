#include "app_emissor.h"

int startControlPacket(int fd) {
    char cPack[255]; //Pacotes de controlo para sinalizar o início e o fim da transferência do ficheiro
    off_t fsize = sizeof(ficheiro.st_size);
    cPack[0] = C_START; //campo de controlo para indicar o start
    cPack[1] = T_SIZE; //parametro, neste caso tamanho do ficheiro
    cPack[2] = fsize; //tamanho em octetos (campo V)
    memcpy(&cPack[3],&ficheiro.st_size,fsize); //valor

    cPack[fsize+3] = T_NAME; //parametro para indicar nome
    cPack[fsize+4] = strlen(app.filename); //tamanho do ficheiro em octetos
    memcpy(&cPack[fsize+5],app.filename,strlen(app.filename)); //nome do ficheiro (pinguim)

    int nbytes = llwrite(fd, cPack, fsize + 5 + strlen(app.filename));
    return nbytes;
}

int endControlPacket(int fd) {

    char cPack[255]; //Pacotes de controlo para sinalizar o início e o fim da transferência do ficheiro
    off_t fsize = sizeof(ficheiro.st_size);
    cPack[0] = C_END; //pacote de controlo final é igual ao inicial à excessão do parametro cPack[0]
    cPack[1] = T_SIZE; //parametro, neste caso tamanho do ficheiro
    cPack[2] = fsize; //tamanho em octetos (campo V)
    memcpy(&cPack[3],&ficheiro.st_size,fsize); //valor

    cPack[fsize+3] = T_NAME; //parametro para indicar nome
    cPack[fsize+4] = strlen(app.filename); //tamanho do ficheiro em octetos
    memcpy(&cPack[fsize+5],app.filename,strlen(app.filename)); //nome do ficheiro (pinguim)


    int nbytes = llwrite(fd, cPack, fsize + 5 + strlen(app.filename));
    return nbytes;
}

int appEmissor(int fd) {
    startClock();

    if (stat(app.filename, &ficheiro)<0){
        printf("Erro no stat em appEmissor()...\n");
        return -1;
    }

    int fdfile = open(app.filename,O_RDONLY); // abre o ficheiro do pinguim
    if (fdfile <0){
        printf("Erro ao abrir ficheiro em appEmissor()...\n");
        return -1;
    }

    int numStartControlPackBytes = startControlPacket(fd);
    if (numStartControlPackBytes < 0) {
        printf("Erro ao escrever pacote de controlo inicial em appEmissor()...\n");
        return -1;
    }
    else printf("Pacote de controlo final: %d bytes escritos\n", numStartControlPackBytes);

    int num = 0;
    char dPack[MAX_SIZE]; //pacote de dados
    char * file_data[MAX_SIZE];//dados do ficheiro
    int bytes_number; //número de bytes
    int numDataPack = 1;

    while((bytes_number = read(fdfile,file_data,MAX_SIZE-4)) != 0){   
        // guarda conteudo em file_data
        // guarda numero de bytes em bytes_number

        dPack[0] = C_DATA; //valor 1, parametro de dados
        dPack[1] = num % 255; //numero de sequencia (modulo de 255)
        //(K = 256 * L2 + L1)
        dPack[2] = bytes_number / 256; //L2
        dPack[3] = bytes_number % 256; //L1 resto
        memcpy(&dPack[4],file_data,bytes_number); //dados

        int numDataPackBytes = llwrite(fd, dPack, bytes_number + 4);
        if (numDataPackBytes < 0){
            //escreve os dados para fd
            printf("Erro ao escrever pacote de dados em appEmissor()...\n");
            return -1;
        }
        else printf("Pacote de dados nº %d: %d bytes escritos\n", numDataPack, numDataPackBytes);

        num++;
        numDataPack++;
    }
    
    int numEndControlPackBytes = endControlPacket(fd);
    if (numEndControlPackBytes < 0) {
        printf("Erro ao escrever pacote de controlo final em appEmissor()...\n");
        return -1;
    }
    else printf("Pacote de controlo final: %d bytes escritos\n", numEndControlPackBytes);

    currentClock_BperSecond(ficheiro.st_size);

    return 0;
}