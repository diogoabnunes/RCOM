#ifndef UTILS_H
#define UTILS_H

#include "data_link.h"

void print_0x(unsigned char a);
void atende();

int llinit(int *fd, char *port);

int emissor_SET(int fd);
int recetor_UA(int fd);

int ciclo_write(int fd, unsigned char *buf, int bufsize);
int ciclo_read(int fd, unsigned char *c, unsigned char **dataBuf, int *size);

int emissor_DISC(int fd);
int recetor_DISC(int fd);

#endif // UTILS_H