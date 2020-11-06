#ifndef EFICIENCY_H
#define EFICIENCY_H

#include <time.h>
#include "data_link.h"

struct timespec start;
struct timespec current;

// Função auxiliar para imprimir a velocidade de transmissão.
int baudrate_number(int b);

// Função que inicia um cronómetro.
void startClock();

/**
 * Função que imprime:
 * - a velocidade de transmissão (BAUDRATE);
 * - o tamanho dos pacotes de transmissão (MAX_SIZE);
 * - o número de bytes total do ficheiro enviado (nbytes);
 * - o tempo que passou desde startClock();
 * - o débito da transmissão (bit/s).
*/
void currentClock_BperSecond(int bytes);

// Função que gera erros aleatórios em tramas de informação no BCC1.
void random_error_BCC1(unsigned char *checkBuf);

// Função que gera erros aleatórios em tramas de informação no BCC2.
void random_error_BCC2(unsigned char *frame, int size);

#endif // EFICIENCY_H