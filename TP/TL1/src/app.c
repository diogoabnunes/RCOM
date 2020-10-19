#include "data_link.h"

int main(int argc, char** argv) {
    printf("Application\n");

    if (argc != 3) {
        printf("Usage: ./app emissor/recetor serialPort");
        exit(1);
    }
    int type;
    if      (strcmp("emissor", argv[1]) == 0) type = EMISSOR;
    else if (strcmp("recetor", argv[1]) == 0) type = RECETOR;
    else {
        printf("Usage: ./app emissor/recetor serialPort");
        exit(1);
    }
    char port[20];
    strcpy(port, argv[2]);

    int fd = llopen(port, type);

    printf("Next\n");
    llclose(fd);
    return 0;
}