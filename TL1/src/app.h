#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "defines.h"
#include "app_emissor.h"
#include "app_recetor.h"
#include "data_link.h"

struct applicationLayer {
  int type;                 // EMISSOR | RECETOR
  char port[255];           // /dev/ttySx
  char filename[255];       // Nome do ficheiro com a mensagem a enviar
  char destination[255];    // Pasta de destino da mensagem a receber

  char file[255];           // Ficheiro
  off_t filesize;           // Tamanho do ficheiro (em bytes)
} app;

// Função que lê os argumentos da linha de comandos e os coloca na struct app.
void parseArguments(int argc, char **argv);

// Função main.
int main(int argc, char** argv);

#endif // APP_H