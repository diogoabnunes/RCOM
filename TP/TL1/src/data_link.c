#include "data_link.h"

struct termios oldtio;
enum stateMachine state;
int fail = FALSE;
struct linkLayer ll;

void print_0x(unsigned char a) {
  printf("Conteúdo: %#4.2x\n" , a);
}

void atende() {
  if (state != SM_STOP) {
    printf("Alarm!\n");
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


void stateMachine_SET_DISC(unsigned char byte, unsigned char A, unsigned char C) {
  static unsigned char checkBuf[2];
  
  switch(state) {
    
    case START:
      if (byte == FLAG) state = FLAG_RCV;
      break;
    
    case FLAG_RCV:
      if (byte == A) {
        state = A_RCV;
        checkBuf[0] = byte;
      }
      else if (byte != FLAG) state = START;
      break;

    case A_RCV: 
        if (byte == C) {
          state = C_RCV;
          checkBuf[1] = byte;
        }
        else if (byte == FLAG) state = FLAG_RCV;
        else state = START;
        break;

    case C_RCV:
      if (byte == BCC(checkBuf[0], checkBuf[1])) state = BCC_OK;
      else if (byte == FLAG) state = FLAG_RCV;
      else state = START;
      break;

    case BCC_OK:
      if (byte == FLAG) state = SM_STOP;
      else state = START;
      break;

    case SM_STOP:
      break;
  }
}

int emissor_SET(int fd) {
  volatile int STOP=FALSE;

  unsigned char buf[SET_UA_SIZE], readBuf[SET_UA_SIZE];
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

    alarm(ll.timeout);

    state = START;
    
    printf("Emissor: Receção de mensagem UA\n");
    // Leitura da resposta UA do recetor
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);
      if (res == -1) {
        if (num_try < ll.numTransmissions) {
          fail = TRUE;
        }
        break;
      }

      print_0x(buf[0]);
      
      stateMachine_SET_DISC(readBuf[0], A_EmiRec, C_UA);

      if (state == SM_STOP || fail) STOP = TRUE;
    }
  } while (num_try < ll.numTransmissions && fail);
  
  alarm(0);
  if (fail) return 1;

  return fd;
}

int recetor_UA(int fd) {
  volatile int STOP=FALSE;

  unsigned char buf[1];
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
    
    stateMachine_SET_DISC(buf[0], A_EmiRec, C_SET);

    if (state == SM_STOP) STOP = TRUE;
  }

  // Resposta do recetor
  unsigned char reply[5];
  reply[0] = FLAG;
  reply[1] = A_EmiRec;
  reply[2] = C_UA;
  reply[3] = BCC(A_EmiRec, C_UA);
  reply[4] = FLAG;

  printf("Recetor: envio de mensagem UA\n");
  res = write(fd, reply, SET_UA_SIZE);
  if (res != 0) {
    printf("Recetor: Erro no envio de mensagem UA\n");
    return 1;
  }

  return fd;
}

int emissor_DISC();

int recetor_DISC();


int llinit(int *fd, char *port) {

    *fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); exit(-1); }

    strcpy(ll.port, port);
    ll.baudRate = BAUDRATE;
    ll.sequenceNumber = 0x00;
    ll.timeout = 3;
    ll.numTransmissions = 3;

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
      case EMISSOR: {
        ll.type = EMISSOR;
        emissor_SET(fd);
        break;
      }
      case RECETOR: {
        ll.type = RECETOR;
        recetor_UA(fd);
        break;
      }
  }
  return 0;
}


int stateMachine_Write(unsigned char byte) {
  static unsigned char checkBuf[2];

  switch(state) {
    case START:
      if (byte == FLAG) state = FLAG_RCV;
      break;

    case FLAG_RCV:
      if (byte == A_EmiRec) {
        state = A_RCV;
        checkBuf[0] = byte;
      }
      else if (byte != FLAG) state = START;
      break;

    case A_RCV:
      if (byte == C_RR(ll.sequenceNumber ^ 0x01)) {
        state = C_RCV;
        checkBuf[1] = byte;
      }
      else if (byte == C_REJ(ll.sequenceNumber ^ 0x01)) return 1;
      else if (byte == FLAG) state = FLAG_RCV;
      else state = START;
      break;

    case C_RCV:
      if (byte == BCC(checkBuf[0], checkBuf[1])) state = BCC_OK;
      else if (byte == FLAG) state = FLAG_RCV;
      else return 1;
      break;

    case BCC_OK:
      if (byte == FLAG) state = SM_STOP;
      else state = START;
      break;

    case SM_STOP:
      break;
  } 

  return 0;
}

int stateMachine_Read(char byte, unsigned char **buf, int *bufSize) {
  static int frameIndex;
  static unsigned char checkBuf[2];
  static int correctNs = TRUE;

  ll.frame[frameIndex] = byte;
  switch(state) {
    case START: {
      frameIndex = 0;
      if (byte == FLAG) {
        state = FLAG_RCV;
        frameIndex++;
      }
      break;
    }

    case FLAG_RCV: 
      if (byte == A_EmiRec) {
        state = A_RCV;
        checkBuf[0] = byte;
        frameIndex++;
      }
      else if (byte != FLAG) state = START;
    break;

    case A_RCV: 
      if (byte == C_I(ll.sequenceNumber)) {
        state = C_RCV;
        checkBuf[1] = byte;
        frameIndex++;
      }
      else if (byte == C_I(ll.sequenceNumber ^ 0x01)) {
        state = C_RCV;
        checkBuf[1] = byte;
        frameIndex++; // -> Wrong Ns
        correctNs = FALSE;
      }
      else if (byte == FLAG) {
        state = FLAG_RCV;
        frameIndex = 1;
      }
      else state = START;
      break;

    case C_RCV:
      if (byte == BCC(checkBuf[0], checkBuf[1])) {
        if (!correctNs) {
          printf("stateMachine_Read(): Já recebeu pacote de leitura\n");
          return 2;
        }
        state = BCC_OK;
        frameIndex++;
      }
      else if (byte == FLAG) {
        state = FLAG_RCV;
        frameIndex = 1;
      }
      else {
        printf("stateMachine_Read(): BCC errado\n");
        return 2;
      }
      break;

    case BCC_OK: {
      frameIndex++;

      if (byte == FLAG) {
        *buf = (unsigned char *)malloc(frameIndex-4-2);
        *bufSize = 0;

        // De-stuffing
        for (int i = 4; i < frameIndex - 2; i++) {
          if (ll.frame[i] == 0x7D || ll.frame[i] == 0x7E) {
            (*buf)[*bufSize] = ll.frame[i + 1] ^ 0x20;
            i++;
          }
          else {
            (*buf)[*bufSize] = ll.frame[i];
          }
          (*bufSize)++;
        }
        *buf = (unsigned char *)realloc(*buf, (*bufSize));
      
        unsigned char BCC2 = (*buf)[0];
        for (int i = 1; i < *bufSize; i++) BCC2 = BCC(BCC2, (*buf)[i]);

        if (ll.frame[frameIndex - 2] == BCC2) {
          ll.sequenceNumber = XOR(ll.sequenceNumber, 0x01);
          state = SM_STOP;
        }
        else {
          printf("stateMachine_Read(): BCC errado\n");
          return 1;
        }
      }
    }
      break;

    case SM_STOP:
      break;
  }
  return 0; 
}

int llwrite(int fd, char *buffer, int length) {
  volatile int STOP = FALSE;

  unsigned char initBuf[4];
  initBuf[0] = FLAG;
  initBuf[1] = A_EmiRec;
  initBuf[2] = C_I(ll.sequenceNumber);
  initBuf[3] = BCC(A_EmiRec, C_I(ll.sequenceNumber));

  unsigned char BCC2 = buffer[0];
  for (int i = 1; i < length; i++) {
    BCC2 = XOR(BCC2, buffer[i]);
  }
  unsigned char endBuf[2];
  endBuf[0] = BCC2;
  endBuf[1] = FLAG;

  int size = length;
  unsigned char *dataBuf = (unsigned char *) malloc(size);
  for (int i = 0, j = 0; i < length; i++, j++) {
    if (buffer[i] == 0x7E || buffer[i] == 0x7D) {
      // Aumentar tamanho do buffer de dados
      size++;
      dataBuf = (unsigned char *) realloc(dataBuf, size);

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

  int num_try = 0, res;
  do {
    num_try++;
    fail = FALSE;

    res = write(fd, allBuf, sizeof(allBuf));
    if (res == -1) {
      printf("ll_write(): Erro a enviar Trama I\n");
      return 1;
    }

    alarm(ll.timeout);
    state = START;

    unsigned char readBuf[255];
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);

      if (res == -1) {
        if (num_try < ll.numTransmissions) {
          fail = TRUE;
        }
        else {
          printf("llwrite(): Erro a ler Trama I\n");
        }
        break;
      }

      stateMachine_Write(readBuf[0]);

      if (state == SM_STOP || fail) STOP = TRUE;
    }

  } while (num_try < ll.numTransmissions && fail);
  alarm(0);
  ll.sequenceNumber = XOR(ll.sequenceNumber, 0x01);

  return 0;
}

int llread(int fd, char *buffer) {
    
    unsigned char buf[1];
    unsigned char *dataBuf;
    volatile int STOP = FALSE;
    state = START;
    int res, size;
    unsigned char cValue;

    printf("\n\nConteúdo em llread():\n");
    while (STOP == FALSE) {
      res = read(fd, buf, 1);
      if (res != 1) {
        printf("llread(): Erro na leitura do Trama I\n");
        return 1;
      }

      int sm_Read = stateMachine_Read(buf[0], &dataBuf, &size);
      if (sm_Read == 1) {
        cValue = C_REJ(ll.sequenceNumber);
        printf("llread(): Erro no BCC\n");
        break;
      }
      else if (sm_Read == 2) {
        cValue = C_RR(ll.sequenceNumber);
        printf("llread(): Valor de Ns errado\n");
        break;
      }
      cValue = C_RR(ll.sequenceNumber);
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
    if (res == -1) {
      printf("llread(): Erro a escrever resposta\n");
      return 1;
    }
    
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