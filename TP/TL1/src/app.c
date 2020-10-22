#include "data_link.h"

int main(int argc, char** argv) {
    printf("\n\n\nApplication\n");

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

    if (type == EMISSOR) printf("Emissor\n\n");
    else printf("Recetor\n\n");

    int fd = llopen(port, type);
    if (fd == -1) {
        printf("Error in llopen()...\n");
        exit(2);
    }
    else printf("llopen() succeeded!\n");

    if (type == EMISSOR) llwrite(fd, "RCOM TEST", 9);
    if (type == RECETOR) {
        int size = llread(fd, "RCOM TEST");
        printf("Size MSG: %d", size);
    }

    if (llclose(fd) != 0) {
        printf("Error in llclose()...\n");
        exit(2);
    }
    else printf("llclose() succeeded!\n");

    return 0;
}