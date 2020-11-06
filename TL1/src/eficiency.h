#ifndef EFICIENCY_H
#define EFICIENCY_H

#include <time.h>
#include "data_link.h"

struct timespec start;
struct timespec current;

// Função auxiliar para imprimir a velocidade de transmissão.
int baudrate_number(int b);

void startClock();
void currentClock_BperSecond(int bytes);

void random_error_BCC1(unsigned char *checkBuf);
void random_error_BCC2(unsigned char *frame, int size);

#endif // EFICIENCY_H