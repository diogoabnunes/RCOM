#ifndef DATA_LINK_H
#define DATA_LINK_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include "state_machine.h"
#include "defines.h"
#include "utils.h"

struct linkLayer {
  unsigned int type;             // EMISSOR | RECETOR
  char port[15];                 // Dispositivo /dev/ttySx
  int baudRate;                  // Velocidade de transmissão
  unsigned char sequenceNumber;  // Número de sequência de trama: 
  unsigned int timeout;          // Valor do temporizador
  unsigned int numTransmissions; // Número de tentativas em caso de falha
  unsigned char frame[MAX_SIZE]; // Trama
};

struct linkLayer ll;             // Protocolo
struct termios oldtio;           // Para usar em llclose
volatile int fail;               // Para usar no alarme

// Retorna identificador da ligação de dados.
// Retorna valor negativo em caso de erro.
int llopen(char *port, int type);

// Retorna o número de caracteres escritos (dos dados e do resultado do byte stuffing).
// Retorna valor negativo em caso de erro.
int llwrite(int fd, char *buffer, int length);

// Retorna o número de caracteres lidos (apenas dos dados, o destuffing é feito na máquina de estados).
// Retorna valor negativo em caso de erro.
int llread(int fd, unsigned char *buffer);

// Retorna valor positivo em caso de sucesso.
// Retorna valor negativo em caso de erro.
int llclose(int fd);

#endif // DATA_LINK_H