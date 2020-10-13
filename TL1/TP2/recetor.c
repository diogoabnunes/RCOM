/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "macros.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

void checkState(enum stateMachine *state, char *checkBuf, char byte) {
  printf("A:%#4.2x C:%#4.2x \n", checkBuf[0], checkBuf[1]);
  switch(*state) {
    
    case START:
      if (byte == FLAG)
        *state = FLAG_RCV;
      break;
    
    case FLAG_RCV:
      if (byte = A_EmiRec) {
        *state = A_RCV;
        checkBuf[0] = byte; // A
      }
      break;

    case A_RCV:
      if (byte = C_SET) {
        *state = C_RCV;
        checkBuf[1] = byte; // C
      }
      else if (byte == FLAG) {
        *state = FLAG_RCV;
      }
      else {
        *state = START;
      }
      break;

    case C_RCV:
      if (byte == BCC(checkBuf[0], checkBuf[1])) {
        *state = BCC_OK;
      }
      else if (byte == FLAG) {
        *state = FLAG_RCV;
      }
      else {
        *state = START;
      }
      break;

    case BCC_OK:
      if (byte == FLAG) {
        *state = SM_STOP;
      }
      else {
        *state = START;
      }
      break;

    case SM_STOP:
      break;
  }
}

int main(int argc, char** argv)
{
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];

  if ( (argc < 2) || ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  	                  (strcmp("/dev/ttyS11", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
    exit(1);
  }

  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {perror(argv[1]); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  /* 
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
  leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  char reply[255], check[2];
  enum stateMachine state = START;

  // Receção da mensagem do emissor
  while (STOP==FALSE) {
    res = read(fd, buf, 1);
    
    printf("nº bytes lido: %d - ", res);
    printf("conteúdo: %#4.2x\n", buf[0]);
    
    checkState(&state, check, buf[0]);

    if (state == SM_STOP) STOP = TRUE;
  }

  // Resposta do recetor
  reply[0] = FLAG;
  reply[1] = A_EmiRec;
  reply[2] = C_UA;
  reply[3] = BCC(A_EmiRec, C_UA);
  reply[4] = FLAG;

  res = write(fd, reply, SET_UA_SIZE);
  print("%d bytes written\n", res);

  tcsetattr(fd,TCSANOW,&oldtio);
  close(fd);
  return 0;
}
