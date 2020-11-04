#ifndef APP_EMISSOR_H
#define APP_EMISSOR_H

#include <sys/stat.h>

#include "defines.h"


// Função que trata do envio dos pacotes de controlo e de dados para o recetor
int appEmissor(int fd);

#endif // APP_EMISSOR_H