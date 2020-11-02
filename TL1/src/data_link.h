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
#include <errno.h>

#include "state_machine.h"
#include "defines.h"
#include "utils.h"

struct linkLayer {
  unsigned int type; // EMISSOR/RECETOR
  char port[15];     // /dev/ttySx
  int baudRate;
  unsigned char sequenceNumber; // Ns: 0/1
  unsigned int timeout;
  unsigned int numTransmissions;
  unsigned char frame[2*MAX_SIZE];
};

struct linkLayer ll;
struct termios oldtio;
volatile int fail;

int llopen(char *port, int type);
int llwrite(int fd, char *buffer, int length);
int llread(int fd, unsigned char *buffer);
int llclose(int fd);

#endif // DATA_LINK_H