#include "data_link.h"

struct applicationLayer{
  off_t filesize;
  char filename[255];
  char destinationArg[255];
  char filenameArg[255];
  int type;
  char port[255];
};

struct applicationLayer applayer;

int main(int argc, char** argv) {
    printf("\n\n\nAplicação - RCOM - TL1\n");

    if (argc != 4) {
        printf("Usage: ./app emissor/recetor serialPort filename/destination");
        exit(1);
    }
    if (strcmp("emissor", argv[1]) == 0) {
        applayer.type = EMISSOR;
        strcpy(applayer.filenameArg, argv[3]);
    }
    else if (strcmp("recetor", argv[1]) == 0) {
        applayer.type = RECETOR;
        strcpy(applayer.destinationArg, argv[3]);
    }

    strcpy(applayer.port, argv[2]);

    if (applayer.type == EMISSOR) printf("Emissor\n\n");
    else if (applayer.type == RECETOR) printf("Recetor\n\n");

    int fd = llopen(applayer.port, applayer.type);
    if (fd < 0) {
        printf("Error in llopen()...\n");
        exit(2);
    }
    
    /* TO DO
    if (applayer.type == EMISSOR) appEmissor(fd, applayer.filenameArg);
    else if (applayer.type == RECETOR) appRecetor(fd);
    */

    if (llclose(fd) < 0) {
        printf("Erro em llclose()\n");
        exit(3);
    }

    if (applayer.type == EMISSOR) printf("Emissor terminou execução.");
    else if (applayer.type == RECETOR) printf("Recetor terminou execução.");

    return 0;
}