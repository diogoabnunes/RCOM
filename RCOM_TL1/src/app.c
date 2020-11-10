#include "app.h"

void parseArguments(int argc, char **argv) {
    if (argc != 4) {
        printf("Uso: ./app emissor portaSerie ficheiro\n");
        printf("Uso: ./app recetor portaSerie destino\n");
        exit(-1);
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
    printf("\n\nAplicação - RCOM - TL1\n");

    parseArguments(argc, argv);

    printf("A inicializar a conexão...\n");
    int fd = llopen(app.port, app.type);
    if (fd < 0) {
        printf("Erro em llopen()...\n");
        return -1;
    }

    if (app.type == EMISSOR) {
        if (appEmissor(fd) < 0) printf("Erro no appEmissor()...\n");
    }
    else if (app.type == RECETOR) {
        if (appRecetor(fd) < 0) printf("Erro no appRecetor()...\n");
    }

    printf("\nA terminar a conexão...\n");
    if (llclose(fd) < 0) {
        printf("Erro em llclose()\n");
        return -1;
    }

    if (app.type == EMISSOR) printf("Emissor terminou execução.\n\n");
    else if (app.type == RECETOR) printf("Recetor terminou execução.\n\n");

    return 0;
}
