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

#define SET_UA_SIZE 5

#define FLAG 0x7E
#define A_EmiRec 0x03
#define A_RecEmi 0x01
#define C_SET 0b00000011 // 0x03
#define C_DISC 0b00001011 // 0x0B
#define C_UA 0b00000111 // 0x07

#define BCC(a,c) (a^c)
#define XOR(a,b) (a^b)

#define C_RR(r) ((0b10000101) ^ (r) << (7)) // 0x05 | 0x85
#define C_REJ(r) ((0b10000001) ^ (r) << (7)) // 0x01 | 0x81
#define C_I(r) ((0b01000000) ^ (r) << (6)) // 0x00 | 0x40

#define BAUDRATE B38400

#define EMISSOR 0
#define RECETOR 1

#define FALSE 0
#define TRUE 1

#define MAX_SIZE 512

enum stateMachine {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    SM_STOP
};

struct linkLayer {
  unsigned int type;
  char port[20];
  int baudRate;
  unsigned char sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  unsigned char frame[MAX_SIZE];
};

void print_0x(unsigned char a);
void atende();
void setting_alarm_handler();

void stateMachine_SET_DISC(unsigned char byte, unsigned char A, unsigned char C);
int emissor_SET(int fd);
int recetor_UA(int fd);

int llinit(int *fd, char *port);
int llopen(char *port, int type);

int stateMachine_Write(unsigned char byte);
int stateMachine_Read(char byte, unsigned char **buf, int *bufSize);
int llwrite(int fd, char *buffer, int length);
int llread(int fd, char *buffer);

int emissor_DISC(int fd);
int recetor_DISC(int fd);
int llclose(int fd);

#endif // DATA_LINK_H