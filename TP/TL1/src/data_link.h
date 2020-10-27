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
#include <errno.h>

#include "state_machine.h"
#include "defines.h"

struct linkLayer {
  unsigned int type;
  char port[20];
  int baudRate;
  unsigned char sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  unsigned char frame[MAX_SIZE];
};

struct linkLayer ll;
struct termios oldtio;
volatile int fail;

void print_0x(unsigned char a);
void atende();
void setting_alarm_handler();

int emissor_SET(int fd);
int recetor_UA(int fd);

int llinit(int *fd, char *port);
int llopen(char *port, int type);

int llwrite(int fd, char *buffer, int length);
int llread(int fd, unsigned char *buffer);

int emissor_DISC(int fd);
int recetor_DISC(int fd);
int llclose(int fd);

#endif // DATA_LINK_H