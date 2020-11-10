#ifndef APP_EMISSOR_H
#define APP_EMISSOR_H

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "app.h"
#include "defines.h"
#include "efficiency.h"

struct stat ficheiro; //stat ficheiro do pinguim

// Função que trata do envio do pacote de controlo inicial.
int startControlPacket(int fd);

//Função que trata do envio do pacote de controlo final.
int endControlPacket(int fd);

// Função que trata do envio dos pacotes de controlo e de dados para o recetor.
int appEmissor(int fd);

#endif // APP_EMISSOR_H