#include "data_link.h"

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
        return emissor_SET(fd);
        break;
      }
      case RECETOR: {
        ll.type = RECETOR;
        return recetor_UA(fd);
        break;
      }
  }
  return 0;
}

int llwrite(int fd, char *buffer, int length) {
  if (length > MAX_SIZE) {
    printf("A mensagem tem mais do que MAX_SIZE...\n");
    return 1;
  }

  unsigned char initBuf[4];
  initBuf[0] = FLAG;
  initBuf[1] = A_EmiRec;
  initBuf[2] = C_I(ll.sequenceNumber);
  initBuf[3] = BCC(A_EmiRec, C_I(ll.sequenceNumber));

  unsigned char BCC2 = buffer[0];
  for (int i = 1; i < length; i++) {
    BCC2 = BCC(BCC2, buffer[i]);
  }

  unsigned char *endBuf = (unsigned char *)malloc(2);
  int endBufSize = 2;
  // Stuffing
  if (BCC2 == 0x7E || BCC2 == 0x7D) {
    endBufSize = 3;
    endBuf = (unsigned char *)realloc(endBuf, endBufSize);
    endBuf[0] = 0x7D;
    endBuf[1] = XOR(BCC2, 0x20);
    endBuf[2] = FLAG;
  }
  else {
    endBuf[0] = BCC2;
    endBuf[1] = FLAG;
  }

  int size = length;
  unsigned char *dataBuf = (unsigned char *) malloc(size);
  for (int i = 0, j = 0; i < length; i++, j++) {
    if (buffer[i] == 0x7E || buffer[i] == 0x7D) {
      // Aumentar tamanho do buffer de dados
      size++;
      dataBuf = (unsigned char *) realloc(dataBuf, size);

      size++;
      dataBuf = (unsigned char *) realloc(dataBuf, size); 

      dataBuf[j+1] = buffer[i] ^ 0x20;
      dataBuf[j] = 0x7D;
      j++;
    }
    else dataBuf[j] = buffer[i];
  }

  // Criação de Trama I: init + dados + end
  int allSize = 4 + size + endBufSize, datai = 0, endi = 0;
  unsigned char allBuf[allSize];
  for (int i = 0; i < allSize; i++) {
    if (i < 4) {
      allBuf[i] = initBuf[i];
    }
    else if (datai < size) {
      allBuf[i] = dataBuf[datai];
      dataBuf++;
    }
    else if (endi < endBufSize) {
      allBuf[i] = endBuf[endi];
      endi++;
    }
  }

  settingUpSM(WRITE, START, A_EmiRec, C_RR(XOR(ll.sequenceNumber, 0x01)));

  int num_try = 0, res;
  volatile int STOP = FALSE;

  // Envio de Trama I
  do {
    num_try++;
    res = write(fd, allBuf, allSize);
    tcflush(fd, TCIFLUSH);
    if (res == -1) {
      printf("ll_write(): Erro a enviar Trama I\n");
      return 1;
    }

    alarm(ll.timeout);
    SM.state = START;
    fail = FALSE;
    unsigned char readBuf[1];

    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);

      if (res == -1 && errno == EINTR) {
        printf("Erro a receber RR do recetor...\n");
        if (num_try < ll.numTransmissions) {
          printf("Nova tentativa...\n");
        }else {
          printf("Excedeu numero de tentativas\n");
          return -1;
        }
      }
      else if (res == -1) {
          printf("Erro a receber RR\n");
          return 1;
      }

      if (stateMachine(readBuf[0], NULL, NULL) < 0) {
        printf("Erro a receber RR ou REJ...\n");
        fail = TRUE;
        alarm(0);
        break;
      }

      if (SM.state == SM_STOP || fail) STOP = TRUE;
    }

  } while (num_try < ll.numTransmissions && fail);
  
  alarm(0);
  ll.sequenceNumber = XOR(ll.sequenceNumber, 0x01);
  free(dataBuf);
  free(endBuf);

  return 0;
}

int llread(int fd, unsigned char *buffer) {
  settingUpSM(READ, START, A_EmiRec, C_I(ll.sequenceNumber));
  unsigned char *dataBuf;
  int size;
  unsigned char cValue;

  volatile int STOP = FALSE;
  unsigned char buf[1];
  int res;

  while (STOP == FALSE) {
    res = read(fd, buf, 1);
    if (res == -1) {
      printf("Erro a receber trama I...\n");
      break;
    }

    int smRead = stateMachine(buf[0], &dataBuf, &size);
    if (smRead == -1) {
      cValue = C_REJ(ll.sequenceNumber);
      printf("Erro no BCC...\n");
      tcflush(fd, TCIFLUSH);
      return -1;
    }
    else if (smRead == -2) {
      cValue = C_RR(ll.sequenceNumber);
      printf("Número de sequência errado em no byte C...\n");
      break;
    }
    cValue = C_RR(ll.sequenceNumber);

    if (SM.state == SM_STOP) STOP = TRUE;
  }

  unsigned char reply[5];
  reply[0] = FLAG;
  reply[1] = A_EmiRec;
  reply[2] = cValue;
  reply[3] = BCC(A_EmiRec, cValue);
  reply[4] = FLAG;

  tcflush(fd, TCOFLUSH);
  res = write(fd, reply, 5);
  if (res == -1) {
    printf("Erro a escrever resposta em llread()...\n");
    return 1;
  }

  for (int i = 0; i < size; i++) {
    buffer[i] = dataBuf[i];
  }
  
  free(dataBuf);
  return size;
}

int llclose(int fd) {
  int ret = fd;

  switch (ll.type) {
    case EMISSOR: if (emissor_DISC(fd) == 0) ret = -1; break;
    case RECETOR: if (recetor_DISC(fd) == 0) ret = -1; break;
  }

  if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    perror("tcsetattr");
    return 1;
  }
  if (close(fd) != 0) {
    printf("Erro em close()\n");
    return 1;
  }

  return ret;
}