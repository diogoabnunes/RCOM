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

#include "macros.h"

#define BAUDRATE B38400

#define EMISSOR 0
#define RECETOR 1

#define FALSE 0
#define TRUE 1

void stateMachine_SET_UA(enum stateMachine *state, unsigned char *checkBuf, char byte, int type);
void atende();

int emissor_SET(int fd);
int recetor_UA(int fd);

void llinit(int *fd, char *port);
int llopen(char *port, int type);
int llwrite(int fd, char *buffer, int length);
int llread(int fd, char *buffer);
int llclose(int fd);

#endif // DATA_LINK_H