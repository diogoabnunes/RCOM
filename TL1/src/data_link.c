#include "data_link.h"

int llopen(char *port, int flag) {
  int fd;

  // Inicialização da conexão
  if (llinit(&fd, port) < 0) {
    printf("Erro em llinit()...\n");
    return -1;
  }

  // Configuração do alarme
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
    printf("O tamanho da mensagem (%d) é maior do que MAX_SIZE (%d)...\n", length, MAX_SIZE);
    return -1;
  }

  // Início de trama I
  unsigned char initBuf[4];
  initBuf[0] = FLAG;
  initBuf[1] = A_EmiRec;
  initBuf[2] = C_I(ll.sequenceNumber);
  initBuf[3] = XOR(A_EmiRec, C_I(ll.sequenceNumber));

  // Fim de trama I
  unsigned char BCC2 = buffer[0];
  for (int i = 1; i < length; i++) {
    BCC2 = XOR(BCC2, buffer[i]);
  }
  unsigned char *endBuf = (unsigned char *)malloc(2);
  int endBufSize = 2;
  // Stuffing no BCC2
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

  // Stuffing nos dados
  int size = length;
  unsigned char *dataBuf = (unsigned char *) malloc(size);
  for (int i = 0, j = 0; i < length; i++, j++) {
    if (buffer[i] == 0x7E || buffer[i] == 0x7D) {
      // Aumentar tamanho do buffer de dados
      size++;
      dataBuf = (unsigned char *) realloc(dataBuf, size);

      dataBuf[j+1] = buffer[i] ^ 0x20;
      dataBuf[j] = 0x7D;
      j++;
    }
    else dataBuf[j] = buffer[i];
  }

  // Construção de Trama I
  int allSize = 4 + size + endBufSize, datai = 0, endi = 0;
  unsigned char allBuf[allSize];
  for (int i = 0; i < allSize; i++) {
    if (i < 4) {
      allBuf[i] = initBuf[i];
    }
    else if (datai < size) {
      allBuf[i] = dataBuf[datai];
      datai++;
    }
    else if (endi < endBufSize) {
      allBuf[i] = endBuf[endi];
      endi++;
    }
  }

  settingUpSM(WRITE, START, A_EmiRec, C_RR(XOR(ll.sequenceNumber, 0x01)));
  
  if (ciclo_write(fd, allBuf, sizeof(allBuf)) < 0) {
    printf("llwrite(): Falha no ciclo write\n");
    return -1;
  }
  
  ll.sequenceNumber = XOR(ll.sequenceNumber, 0x01);

  free(dataBuf);
  free(endBuf);

  return sizeof(allBuf);
}

int llread(int fd, unsigned char *buffer) {
  settingUpSM(READ, START, A_EmiRec, C_I(ll.sequenceNumber));
  unsigned char *dataBuf;
  int size;
  unsigned char cValue;

  if (ciclo_read(fd, &cValue, &dataBuf, &size) < 0) {
    printf("Erro no ciclo read\n");
    return -1;
  }

  unsigned char reply[5];
  reply[0] = FLAG;
  reply[1] = A_EmiRec;
  reply[2] = cValue;
  reply[3] = XOR(A_EmiRec, cValue);
  reply[4] = FLAG;

  int res = write(fd, reply, 5);
  if (res == -1) {
    printf("Erro a escrever resposta em llread()...\n");
    return -1;
  }

  for (int i = 0; i < size; i++) {
    buffer[i] = dataBuf[i];
  }
  
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