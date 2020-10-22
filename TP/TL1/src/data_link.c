#include "data_link.h"

struct termios oldtio;
enum stateMachine state;
int fail = FALSE;
struct linkLayer linkLayer;

void print_0x(unsigned char a) {
  printf("Conteúdo: %#4.2x\n" , a);
}


void stateMachine_SET_UA(enum stateMachine *state, unsigned char *checkBuf, char byte, int type) {
  switch(*state) {
    
    case START:
      if (byte == FLAG) *state = FLAG_RCV;
      break;
    
    case FLAG_RCV:
      if (byte == A_EmiRec) {
        *state = A_RCV;
        checkBuf[0] = byte;
      }
      else if (byte != FLAG) state = START;
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
        else *state = START;
        break;
    }

    case C_RCV:
      if (byte == BCC(checkBuf[0], checkBuf[1])) *state = BCC_OK;
      else if (byte == FLAG) *state = FLAG_RCV;
      else *state = START;
      break;

    case BCC_OK:
      if (byte == FLAG) *state = SM_STOP;
      else *state = START;
      break;

    case SM_STOP:
      break;
  }
}

int stateMachine_Write(enum stateMachine *state, unsigned char byte) {
  static unsigned char checkBuf[2];

  switch(*state) {
    case START:
      if (byte == FLAG) *state = FLAG_RCV;
      break;

    case FLAG_RCV:
      if (byte == A_EmiRec) {
        *state = A_RCV;
        checkBuf[0] = byte;
      }
      else if (byte != FLAG) {
        *state = START;
      }
      break;

    case A_RCV:
      if (byte == C_RR(linkLayer.sequenceNumber ^ 0x01)) {
        *state = C_RCV;
        checkBuf[1] = byte;
      }
      else if (byte == C_REJ(linkLayer.sequenceNumber ^ 0x01)) return 1;
      else if (byte == FLAG) *state = FLAG_RCV;
      else *state = START;
      break;

    case C_RCV:
      if (byte == BCC(checkBuf[0], checkBuf[1])) *state = BCC_OK;
      else if (byte == FLAG) *state = FLAG_RCV;
      else return 1;
      break;

    case BCC_OK:
      if (byte == FLAG) *state = SM_STOP;
      else *state = START;
      break;

    case SM_STOP: break;
  } 

  return 0;
}

int stateMachine_Read(enum stateMachine *state, char byte, unsigned char **buf, int *bufSize) {
  static int frameIndex;
  static unsigned char checkBuf[2];

  linkLayer.frame[frameIndex] = byte;
  switch(*state) {
    case START: {
      frameIndex = 0;
      if (byte == FLAG) {
        *state = FLAG_RCV;
        frameIndex++;
      }
      break;
    }

    case FLAG_RCV: 
      if (byte == A_EmiRec) {
        *state = A_RCV;
        checkBuf[0] = byte;
        frameIndex++;
      }
      else if (byte != FLAG) *state = START;
    break;

    case A_RCV: 
      if (byte == C_I(linkLayer.sequenceNumber)) {
        *state = C_RCV;
        checkBuf[1] = byte;
        frameIndex++;
      }
      else if (byte == C_I(linkLayer.sequenceNumber ^ 0x01)) return 1;
      else if (byte == FLAG) {
        *state = FLAG_RCV;
        frameIndex = 1;
      }
      else *state = START;
      break;

    case C_RCV:
      if (byte == BCC(checkBuf[0], checkBuf[1])) {
        *state = BCC_OK;
        frameIndex++;
      }
      else if (byte == FLAG) {
        *state = FLAG_RCV;
        frameIndex = 1;
      }
      else return 1;
      break;

    case BCC_OK: {
      frameIndex++;

      if (byte == FLAG) {
        *buf = (unsigned char *)malloc(frameIndex-6);
        *bufSize = 0;

        // De-stuffing
        for (int i = 4; i < frameIndex - 2; i++) {
          if (linkLayer.frame[i] != 0x7D || linkLayer.frame[i] != 0x7E) {
            (*buf)[*bufSize] = linkLayer.frame[i];
          }
          else {
            (*buf)[*bufSize] = linkLayer.frame[i + 1] ^ 0x20;
            i++;
          }
          (*bufSize)++;
        }
        *buf = (unsigned char *)realloc(*buf, *bufSize);
      
        unsigned char BCC2 = *buf[0];
        for (int i = 1; i < *bufSize; i++) BCC2 = XOR(BCC2, *buf[i]);

        if (linkLayer.frame[frameIndex - 2] == BCC2) {
          linkLayer.sequenceNumber = XOR(linkLayer.sequenceNumber, 0x01);
          *state = SM_STOP;
        }
        else return 1;
      }
    }
    break;

    case SM_STOP: break;
  }
  return 0; 
}


void atende() {
  if (state != SM_STOP) {
    printf("Emissor não recebeu resposta do recetor!\n");
    fail = TRUE;
    return;
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
  int res, num_try = 0;

  buf[0] = FLAG;
  buf[1] = A_EmiRec;
  buf[2] = C_SET;
  buf[3] = BCC(A_EmiRec, C_SET);
  buf[4] = FLAG;

  do {
    num_try++;
    fail = FALSE;

    printf("Emissor: Envio de mensagem SET\n");
    // Tentativa de enviar mensagem SET
    res = write(fd, buf, SET_UA_SIZE);
    printf("%d bytes written\n", res);

    alarm(linkLayer.timeout);

    state = START;
    
    printf("Emissor: Receção de mensagem UA\n");
    // Leitura da resposta UA do recetor
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);
      if (res == -1) {
        if (num_try < linkLayer.numTransmissions) {
          fail = TRUE;
        }
        break;
      }

      print_0x(buf[0]);
      
      stateMachine_SET_UA(&state, checkBuf, readBuf[0], EMISSOR);

      if (state == SM_STOP || fail) STOP = TRUE;
    }
  } while (num_try < linkLayer.numTransmissions || fail);
  
  alarm(0);
  if (fail) return 1;

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
    if (res == -1) {
      printf("Erro a receber mensagem SET\n");
      return 1;
    }
    
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


int llinit(int *fd, char *port) {

    *fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); exit(-1); }

    strcpy(linkLayer.port, port);
    linkLayer.baudRate = BAUDRATE;
    linkLayer.sequenceNumber = 0x00;
    linkLayer.timeout = 3;
    linkLayer.numTransmissions = 3;

    struct termios newtio;

    if ( tcgetattr(*fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      return 1;
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
        return 1;
    }

    printf("New termios structure set\n");

    return 0;
}

int llopen(char *port, int flag) {
  int fd;
  if (llinit(&fd, port) != 0) {
    printf("Error in llinit()...\n");
    return 1;
  }

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
  initBuf[2] = C_I(linkLayer.sequenceNumber);
  initBuf[3] = BCC(A_EmiRec, C_I(linkLayer.sequenceNumber));

  unsigned char endBuf[2];
  unsigned char BCC2 = buffer[0];
  for (int i = 1; i < length; i++) {
    BCC2 = XOR(BCC2, buffer[i]);
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
  int allSize = 4 + size + 2, datai = 0, endi;
  unsigned char allBuf[allSize];
  for (int i = 0; i < allSize; i++) {
    if (i < 4) {
      allBuf[i] = initBuf[i];
    }
    else if (datai < size) {
      allBuf[i] = dataBuf[i];
      dataBuf++;
    }
    else if (endi < 2) {
      allBuf[i] = endBuf[i];
      endi++;
    }
  }

  printf("\n\nConteúdo de llwrite() para enviar:\n");
  for (int i = 0; i < size; i++) {
    print_0x(allBuf[i]);
  }
  printf("\n");

  int num_try = 0, res;
  do {
    num_try++;
    fail = FALSE;

    res = write(fd, allBuf, sizeof(allBuf));
    if (res == -1) return 1;
    printf("%d bytes written\n", res);

    alarm(linkLayer.timeout);

    unsigned char readBuf[255];
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);

      if (res == -1) {
        if (num_try < linkLayer.numTransmissions) {
          fail = TRUE;
        }
        break;
      }

      stateMachine_Write(&state, readBuf[0]);

      if (state == SM_STOP || fail) STOP = TRUE;
    }

  } while (num_try < linkLayer.numTransmissions && fail);
  alarm(0);
  linkLayer.sequenceNumber ^= 0x01;

  return 0;
}

int llread(int fd, char *buffer) {
    
    unsigned char buf[SET_UA_SIZE];
    unsigned char *dataBuf;
    volatile int STOP = FALSE;
    state = START;
    int res, size;
    unsigned char cValue;

    printf("\n\nConteúdo em llread():\n");
    while (STOP == FALSE) {
      res = read(fd, buf, 1);
      if (res != 1) return 1;

      print_0x(buf[0]);

      if (stateMachine_Read(&state, buf[0], &dataBuf, &size) != 0) {
        cValue = C_REJ(linkLayer.sequenceNumber);
        printf("C_REJ...\n");
        break;
      }
      cValue = C_RR(linkLayer.sequenceNumber);
      if (state == SM_STOP) STOP = TRUE;
    }

    for (int i = 0; i < size; i++) {
      buffer[i] = dataBuf[i];
    }

    unsigned char reply[5];
    reply[0] = FLAG;
    reply[1] = A_EmiRec;
    reply[2] = cValue;
    reply[3] = BCC(A_EmiRec, cValue);
    reply[4] = FLAG;

    printf("\n\nResposta RR: \n");
    for (int i = 0; i < 5; i++) {
      print_0x(reply[i]);
    }

    res = write(fd, reply, 5);
    if (res == -1) return 1;
    
    linkLayer.sequenceNumber = XOR(linkLayer.sequenceNumber, 0x01);
    free(dataBuf);
    return size;
}

int llclose(int fd) {
    // mensagens DISC e UA
    sleep(1);

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}