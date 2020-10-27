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

enum readingType {openR, readR, closeDISC, closeUA}; // Enum of possible modifications to readingCycle()
enum writingType {trans_SET, writeR, trans_DISC_UA}; // Enum of possible modifications to writeCycle()

/**
 * @brief Function to read a byte from fd
 * 
 * @param type variable to distinguish warning messages
 * @param fd file descriptor
 * @param c controll byte, used with type readR
 * @param dataBuf buffer to read data, used with type readR
 * @param retBufferSize variable to store dataBuf size, used with type readR
 * @return int 
 */
int readingCycle(enum readingType type, int fd, unsigned char *c, unsigned char **dataBuf, int *retBufferSize);
/**
 * @brief Function used to write to fd
 * 
 * @param type variable to distinguish warning messages
 * @param fd file descriptor
 * @param buf buffer of content to write
 * @param bufsize lenght of buffer in bytes
 * @return int negative in case of errors, 0 otherwise
 */
int writeCycle(enum writingType type, int fd, unsigned char *buf, int bufsize);

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
int llread(int fd, char *buffer);

int emissor_DISC(int fd);
int recetor_DISC(int fd);
int llclose(int fd);

#endif // DATA_LINK_H