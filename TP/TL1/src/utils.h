#ifndef UTILS_H
#define UTILS_H

#include "data_link.h"

void print_0x(unsigned char a);
void atende();
void setting_alarm_handler();

int llinit(int *fd, char *port);

int emissor_SET(int fd);
int recetor_UA(int fd);

int emissor_DISC(int fd);
int recetor_DISC(int fd);

#endif // UTILS_H