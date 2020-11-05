#ifndef UTILS_H
#define UTILS_H

#include "data_link.h"

// Função auxiliar para imprimir a velocidade de transmissão.
int baudrate_number(int b);

// Função para imprimir um valor em hexadecimal.
void print_0x(unsigned char a);

// Função do alarm handler.
void atende();

// Função que configura o alarm handler.
int setting_alarm_handler();

// Inicialização da conexão.
int llinit(int *fd, char *port);

// Função auxiliar do ciclo write em llwrite.
int ciclo_write(int fd, unsigned char *buf, int bufsize);

// Função auxiliar do ciclo read em llread.
int ciclo_read(int fd, unsigned char *c, unsigned char **dataBuf, int *size);

// Função que envia Trama SET e recebe Trama UA
int emissor_SET(int fd);

// Função que recebe Trama SET e envia Trama UA
int recetor_UA(int fd);

// Função que envia Trama DISC, recebe Trama DISC e envia Trama UA.
int emissor_DISC(int fd);

// Função que recebe Trama DISC, envia Trama DISC e recebe Trama UA.
int recetor_DISC(int fd);

#endif // UTILS_H