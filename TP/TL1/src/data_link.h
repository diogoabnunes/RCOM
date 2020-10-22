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
#include <ctype.h>

#include "macros.h"

#define BAUDRATE B38400

#define EMISSOR 0
#define RECETOR 1

#define FALSE 0
#define TRUE 1

#define MAX_SIZE 512

struct linkLayer {
  char port[20];
  int baudRate;
  unsigned char sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  unsigned char frame[MAX_SIZE];
};

void print_0x(unsigned char a);
void stateMachine_SET_UA(enum stateMachine *state, unsigned char *checkBuf, char byte, int type);
int stateMachine_Write(enum stateMachine *state, unsigned char byte);
int stateMachine_Read(enum stateMachine *state, char byte, unsigned char **buf, int *bufSize);
void atende();

void setting_alarm_handler();
int emissor_SET(int fd);
int recetor_UA(int fd);

int llinit(int *fd, char *port);
int llopen(char *port, int type);
int llwrite(int fd, char *buffer, int length);
int llread(int fd, char *buffer);
int llclose(int fd);

#endif // DATA_LINK_H