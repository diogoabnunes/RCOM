#include "data_link.h"

int main(int argc, char** argv) {
    printf("\n\n\nAplicação\n");

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
    
    /*
    if (type == EMISSOR) llwrite(fd, "RCOM TEST", strlen("RCOM TEST"));
    if (type == RECETOR) {
        int size = llread(fd, "RCOM TEST");
        printf("Size MSG: %d", size);
    }
    */
    if (type == EMISSOR) printf("\nllwrite() to fix\n\n");
    else if (type == RECETOR) printf("\nllread() to fix\n\n");

    if (llclose(fd) != 0) {
        printf("Erro em llclose()\n");
        exit(3);
    }

    return 0;
}