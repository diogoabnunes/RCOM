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
    
    int max = 32;
    int num = 0;
    
    struct stat ficheiro;

    if (stat(app.filename, &ficheiro)<0){
        printf("error appEmissor.\n");
        return -1;
    }

    int fdfile = open(app.filename,O_RDONLY);
    if (fdfile <0){
        printf("error appEmissor.\n");
        return -1;
    }


    char cPack[255];
    int size = sizeof(ficheiro.st_size);
    cPack[0] = 0x02;
    cPack[1] = 0x00;
    cPack[2] = size; 
    memcpy(&cPack[3],&ficheiro.st_size,size);

    cPack[size+3] = 0x01;
    cPack[size+4] = strlen(app.filename);


    memcpy(&cPack[size+5],app.filename,strlen(app.filename));

    if(llwrite(fd,cPack,size + 5 + strlen(app.filename))<0){
        printf("error appEmissor");
        return -1;
    }



    char dPack[max];
    char * file_data[max];
    int bytes_number;
    while( (bytes_number = read(fdfile,file_data,max-4)) != 0){
        
        dPack[0] = 0x01;
        dPack[1] = num % 255;
        dPack[2] = bytes_number / 256;
        dPack[3] = bytes_number % 256;
        memcpy(&dPack[4],file_data,bytes_number);


        if (llwrite(fd,dPack,bytes_number+4) < 0){
        printf("error appEmissor");
        return -1;
        }


        num++;
        

    }

    cPack[0] = 0x03;

    if(llwrite(fd,cPack,size + 5 + strlen(app.filename))<0){
        printf("error appEmissor");
        return -1;
    }


    return 0;
}



void file_content(unsigned char *pack, int psize){
  off_t size = 0;
  int counter = 1;

  while (counter != psize)
  {
    if (pack[counter]==0x00){
      int info = pack[counter+1];
      for (int i = counter+2, k=0; i < info+counter+2; i++, k++){
        size += pack[i] << 8*k;
      }
      app.filesize = size;
      counter=info+3;
    }

    if (pack[counter]==0x01){
      int info = pack[counter+1];
      for (int i =counter+2, k=0; i < info+counter+2; i++, k++){
        app.filename[k] = pack[i];
      }
      app.filename[info+counter+2] = '\0';
      counter = counter+2 + info;
    }
  }
}

int appRecetor(int fd){
    int max = 32;
    unsigned char pack[max];
    
    
    

    int fdfile;

    while(1){
        int lido = llread(fd,pack);
        int psize;
        
        if(lido<0)
            printf("error appRecetor");

        if (pack[0] == 0x02){
            file_content(pack, lido);
            strcat(app.destination,app.filename);
            fdfile = open(app.destination,O_RDWR | O_CREAT, 0777);
            continue;
        }
        else if (pack[0] == 0x01) {
            psize = pack[3] + pack[2] * 256;

            int counter=0, counter2 = 0;
            int move = 0;
            
            if (pack[1] != counter){
                off_t offset = (pack[1] + move) * (max-4);
                lseek(fdfile, offset, SEEK_SET);
            }
            
            write(fdfile,&pack[4], psize);

            if (pack[1] != counter){
                lseek(fdfile, 0 , SEEK_HOLE);
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

        else if (pack[0] == 0x03){
            break;
        }
        
    }

    if (close(fdfile)<0){
        printf("Error closing file\n");
    }
    
    return fd;
}




int main(int argc, char** argv) {
    printf("\n\n\nAplicação - RCOM - TL1\n");

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

    int fd = llopen(app.port, app.type);
    if (fd < 0) {
        printf("Error in llopen()...\n");
        exit(2);
    }
    

    if (app.type == EMISSOR) appEmissor(fd, app.filename);

    /* TO DO
    else if (app.type == RECETOR) appRecetor(fd);
    */

    if (llclose(fd) < 0) {
        printf("Erro em llclose()\n");
        exit(3);
    }

    if (app.type == EMISSOR) printf("Emissor terminou execução.");
    else if (app.type == RECETOR) printf("Recetor terminou execução.");

    return 0;
}

