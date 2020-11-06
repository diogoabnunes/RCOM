#include "eficiency.h"

int baudrate_number(int b) {
    switch(b) {
        case B0: return 0;
        case B50: return 50;
        case B75: return 75;
        case B110: return 110;
        case B134: return 134;
        case B150: return 150;
        case B200: return 200;
        case B300: return 300;
        case B600: return 600;
        case B1200: return 1200;
        case B1800: return 1800;
        case B2400: return 2400;
        case B4800: return 4800;
        case B9600: return 9600;
        case B19200: return 19200;
        case B38400: return 38400;
        default: return -1;
    }
}

void startClock() {
    clock_gettime(CLOCK_MONOTONIC, &start);
}

void currentClock_BperSecond(int bytes) {
    clock_gettime(CLOCK_MONOTONIC, &current);
    double time = (current.tv_sec - start.tv_sec)*1000 + (current.tv_nsec - start.tv_nsec)/10e6;
    double bits = bytes / (time / 1000);

    printf("\nVelocidade de transmissão: %d\n", baudrate_number(ll.baudRate));
    printf("Tamanho dos pacotes de informação: %d\n", MAX_SIZE);
    printf("Número de bytes: %d\n", bytes);
    printf("Tempo: %f ms\n", time);
    printf("Bits por segundo: %f\n", bits);
}

void random_error_BCC1(unsigned char *checkBuf) {
    int probability = (rand() % 100) + 1;

    if (probability <= BCC1_PROB_ERROR) {
        int i = rand() % 2;
        unsigned char random = (unsigned char)(rand() % 70);
        checkBuf[i] = random;
        printf("BCC1 gerado com erros!\n"); 
    }
}

void random_error_BCC2(unsigned char *frame, int size) {
    int probability = (rand() % 100) + 1;

    if (probability <= BCC2_PROB_ERROR) {
        int i = (rand() % (size - 5)) + 4;
        unsigned char random = (unsigned char)(rand() % 70);
        frame[i] = random;
        printf("BCC2 gerado com erros!\n"); 
    }
}