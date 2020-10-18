#include "data_link.h"

struct termios oldtio;
enum stateMachine state;
int fail = FALSE;

void stateMachine_SET_UA(enum stateMachine *state, unsigned char *checkBuf, char byte, int type) {
  printf("A:%#4.2x C:%#4.2x \n", checkBuf[0], checkBuf[1]);
  switch(*state) {
    
    case START:
      if (byte == FLAG)
        *state = FLAG_RCV;
      break;
    
    case FLAG_RCV:
      if (byte == A_EmiRec) {
        *state = A_RCV;
        checkBuf[0] = byte;
      }
      break;

    case A_RCV: {
        int C;
        switch (type) {
            case EMISSOR: C = C_UA; break;
            case RECETOR: C = C_SET; break;
        }

        if (byte == C) {
        *state = C_RCV;
        checkBuf[1] = byte;
        }
        else if (byte == FLAG) {
            *state = FLAG_RCV;
        }
        else {
            *state = START;
        }
        break;
    }

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

void atende() {
  if (state != SM_STOP) {
    printf("Emissor não recebeu resposta do recetor!\n");
    fail = TRUE;
  }
}

void llinit(int *fd, char *port) {

    fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); exit(-1); }

    struct termios newtio;

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

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
}

int llopen(int port, int flag) {
    // llinit();
    // Instala rotina que atende interrupção
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = atende;
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);
    // mensagens SET and UA
    switch (flag) {
        case EMISSOR: break;
        case RECETOR: break;
    }
    return 0;
}

int llwrite(int fd, char *buffer, int length) {
    // enviar tramas/frames
    return 0;
}

int llread(int fd, char *buffer) {
    // receber tramas/frames
    return 0;
}

int llclose(int fd) {
    // mensagens DISC e UA

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}