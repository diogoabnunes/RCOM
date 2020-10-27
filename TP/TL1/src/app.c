#include "data_link.h"

struct applicationLayer {
  int type;
  char port[255];
  char filename[255];
  char destination[255];

  char file[255];
  off_t filesize;
} app;

// int appEmissor(int fd, char *file) {}
// int appRecetor(int fd) {}

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
    
    /* TO DO
    if (app.type == EMISSOR) appEmissor(fd, app.filename);
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