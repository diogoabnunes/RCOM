#ifndef APP_RECETOR_H
#define APP_RECETOR_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "app.h"
#include "defines.h"
#include "efficiency.h"

// Função que guarda o conteúdo do pacote de dados recebido no ficheiro (app.file).
void file_content(unsigned char *pack, int psize);

// Função que trata da receção dos pacotes de controlo e de dados do emissor.
int appRecetor(int fd);

#endif // APP_RECETOR_H