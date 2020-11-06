#ifndef APP_EMISSOR_H
#define APP_EMISSOR_H

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "app.h"
#include "defines.h"
#include "eficiency.h"

// Função que trata do envio dos pacotes de controlo e de dados para o recetor.
int appEmissor(int fd);

#endif // APP_EMISSOR_H