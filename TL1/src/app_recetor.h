#ifndef APP_RECETOR_H
#define APP_RECETOR_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "app.h"
#include "defines.h"
#include "eficiency.h"

// 
void file_content(unsigned char *pack, int psize);

// 
int appRecetor(int fd);

#endif // APP_RECETOR_H