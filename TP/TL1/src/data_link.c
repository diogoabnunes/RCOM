#include "data_link.h"

struct termios oldtio;
enum stateMachine state;
int fail = FALSE;

void print_0x(unsigned char a) {
  printf("Conteúdo: %#4.2x\n" , a);
}

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

void setting_alarm_handler() {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = atende;
  sa.sa_flags = 0;
  sigaction(SIGALRM, &sa, NULL);
}

int emissor_SET(int fd) {
  volatile int STOP=FALSE;

  unsigned char buf[SET_UA_SIZE], readBuf[SET_UA_SIZE], checkBuf[2];
  int res;
  int num_try = 0;

  buf[0] = FLAG;
  buf[1] = A_EmiRec;
  buf[2] = C_SET;
  buf[3] = BCC(A_EmiRec, C_SET);
  buf[4] = FLAG;

  while (num_try < NUM_TRIES || fail == TRUE) {
    num_try++;
    fail = FALSE;

    printf("Emissor: Envio de mensagem SET\n");
    // Tentativa de enviar mensagem SET
    res = write(fd, buf, SET_UA_SIZE);
    printf("%d bytes written\n", res);

    alarm(3);

    state = START;
    
    printf("Emissor: Receção de mensagem UA\n");
    // Leitura da resposta UA do recetor
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);
    
      print_0x(buf[0]);
      
      stateMachine_SET_UA(&state, checkBuf, readBuf[0], EMISSOR);

      if (state == SM_STOP || fail) STOP = TRUE;
    }
  }

  return fd;
}

int recetor_UA(int fd) {
  volatile int STOP=FALSE;

  unsigned char buf[SET_UA_SIZE], checkBuf[2], reply[SET_UA_SIZE];
  int res;

  state = START;

  printf("Recetor: Receção de mensagem SET\n");
  while (STOP==FALSE) {
    res = read(fd, buf, 1);
    
    print_0x(buf[0]);
    
    stateMachine_SET_UA(&state, checkBuf, buf[0], RECETOR);

    if (state == SM_STOP) STOP = TRUE;
  }

  // Resposta do recetor
  reply[0] = FLAG;
  reply[1] = A_EmiRec;
  reply[2] = C_UA;
  reply[3] = BCC(A_EmiRec, C_UA);
  reply[4] = FLAG;

  printf("Recetor: envio de mensagem UA\n");
  res = write(fd, reply, SET_UA_SIZE);
  printf("%d bytes written\n", res);

  return fd;
}

void llinit(int *fd, char *port) {

    *fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); exit(-1); }

    struct termios newtio;

    if ( tcgetattr(*fd,&oldtio) == -1) { /* save current port settings */
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

    tcflush(*fd, TCIOFLUSH);

    if ( tcsetattr(*fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
}

int llopen(char *port, int flag) {
  int fd;
  llinit(&fd, port);

  setting_alarm_handler();

  switch (flag) {
      case EMISSOR: emissor_SET(fd); break;
      case RECETOR: recetor_UA(fd); break;
  }
  return 0;
}

int llwrite(int fd, char *buffer, int length) {
  volatile int STOP = FALSE;

  unsigned char initBuf[4];
  initBuf[0] = FLAG;
  initBuf[1] = A_EmiRec;
  initBuf[2] = C_I(1);
  initBuf[3] = BCC(A_EmiRec, C_I(1));

  unsigned char endBuf[2];
  unsigned char BCC2 = buffer[0];
  for (int i = 1; i < length; i++) {
    BCC2 = BCC(BCC2, buffer[i]);
  }
  endBuf[0] = BCC2;
  endBuf[1] = FLAG;

  int size = length;
  unsigned char *dataBuf = (unsigned char *) malloc(sizeof(char) * size);
  for (int i = 0, j = 0; i < length; i++, j++) {
    if (buffer[i] == 0x7E || buffer[i] == 0x7D) {
      // Aumentar tamanho do buffer de dados
      size++;
      dataBuf = (unsigned char *) realloc(dataBuf, sizeof(char) * size);

      // Stuffing
      switch(buffer[i]) {
        case 0x7E: {
          dataBuf[j] = 0x7E;
          dataBuf[j + 1] = XOR(buffer[i], 0x20); 
          j++;
          break;
        }
        case 0x7D: {
          dataBuf[j] = 0x7D;
          dataBuf[j + 1] = XOR(buffer[i], 0x20);
          j++;
          break;
        }
      }
    }
    else dataBuf[j] = buffer[i];
  }

  /* Trama completo */
  unsigned char allBuf[4 + size + 2];
  strcpy((char *)allBuf, (char *)initBuf);
  strcpy((char *)allBuf, (char *)dataBuf);
  strcpy((char *)allBuf, (char *)endBuf);

  int num_try = 0, res;
  while (num_try < NUM_TRIES && fail) {
    num_try++;
    fail = FALSE;

    res = write(fd, allBuf, sizeof(allBuf));
    printf("%d bytes written\n", res);

    alarm(3);

    unsigned char readBuf[255];
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);

      printf("nº bytes lido: %d - ", res);
      print_0x(readBuf[0]);

      // TO DO : outra stateMachine 

      if (fail) STOP = TRUE;
    }

  }

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