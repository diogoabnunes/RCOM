#include "data_link.h"
#include "state_machine.h"

struct termios oldtio;
struct stateMachine state;
int fail = FALSE;

void print_0x(unsigned char a) {
  if (a == 0) {
    printf("0x00 "); return;
  }
  printf("%#4.2x " , a);
}

void atende() {
  if (SM.state != SM_STOP) {
    fail = TRUE;
    printf("Alarm!\n");
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
  unsigned char buf[SET_UA_SIZE];
  buf[0] = FLAG;
  buf[1] = A_EmiRec;
  buf[2] = C_SET;
  buf[3] = BCC(A_EmiRec, C_SET);
  buf[4] = FLAG;

  settingUpSM(SV, START, A_RecEmi, C_UA);

  volatile int STOP=FALSE;
  int res, num_try = 0;
  do {
    num_try++;
    res = write(fd, buf, SET_UA_SIZE);
    if (res == -1) {
      printf("emissor_SET(): Erro no envio de mensagem SET\n");
      return 1;
    }
    else printf("Mensagem SET enviada\n");

    alarm(ll.timeout);
    SM.state = START;
    fail = FALSE;
    unsigned char readBuf[SET_UA_SIZE];

    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);
      if (res == -1 && errno == EINTR) {
        printf("Erro a receber a mensagem UA do recetor...\n");
        if (num_try < ll.numTransmissions) printf("Nova tentativa...\n");
      }
      else if (res == -1) {
          printf("Erro a receber a mensagem UA do buffer...\n");
      }
      
      stateMachine(readBuf[0], NULL, NULL); // **buf and *size not needed here

      if (SM.state == SM_STOP || fail) STOP = TRUE;
    }
  } while (num_try < ll.numTransmissions && fail);
  printf("Mensagem UA recebida\n");
  printf("\n\n");
  
  alarm(0);
  if (fail) return 1;

  return fd;
}

int recetor_UA(int fd) {
  settingUpSM(SV, START, A_EmiRec, C_SET);

  unsigned char buf[1];
  int res;
  volatile int STOP=FALSE;

  while (STOP==FALSE) {
    res = read(fd, buf, 1);
    if (res == -1) {
      printf("Erro a receber mensagem SET\n");
      return 1;
    }
    
    stateMachine(buf[0], NULL, NULL); // **buf and *size not needed here

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  printf("Mensagem SET recebida\n");

  // Resposta do recetor
  unsigned char reply[SET_UA_SIZE];
  reply[0] = FLAG;
  reply[1] = A_EmiRec;
  reply[2] = C_UA;
  reply[3] = BCC(A_EmiRec, C_UA);
  reply[4] = FLAG;

  res = write(fd, reply, SET_UA_SIZE);
  if (res == -1) {
    printf("recetor_UA(): Erro no envio de mensagem UA\n");
    return 1;
  }
  else printf("Mensagem UA enviada\n");

  return fd;
}


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

int llwrite(int fd, char *buffer, int length) {/*
  printf("A entrar em llwrite()\n");
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
  ll.sequenceNumber = XOR(ll.sequenceNumber, 0x01);*/

  return 0;
}

int llread(int fd, char *buffer) {/*
  printf("A entrar em llread()\n");
    
  unsigned char buf[1];
  unsigned char *dataBuf;
  volatile int STOP = FALSE;
  state = START;
  int res, size;
  unsigned char cValue;

  printf("\n\nConteúdo em llread():\n");
  while (STOP == FALSE) {
    res = read(fd, buf, 1);
    if (res == -1) {
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
  return size;*/
  return 0;
}


int emissor_DISC(int fd) {
  unsigned char msgDISC[SET_UA_SIZE];
  msgDISC[0] = FLAG;
  msgDISC[1] = A_EmiRec;
  msgDISC[2] = C_DISC;
  msgDISC[3] = BCC(A_EmiRec, C_DISC);
  msgDISC[4] = FLAG;

  settingUpSM(SV, START, A_RecEmi, C_DISC);

  int res, num_try = 0;
  volatile int STOP=FALSE;

  do {
    num_try++;
    res = write(fd, msgDISC, SET_UA_SIZE);
    if (res == -1) {
      printf("emissor_DISC(): Erro no envio de mensagem DISC\n");
      return 1;
    }
    else {
      printf("Mensagem DISC enviada: ");
      for (int i = 0; i < SET_UA_SIZE; i++) print_0x(msgDISC[i]);
      printf("\n");
    }

    alarm(ll.timeout);
    SM.state = START;
    fail = FALSE;
    unsigned char readMsgDISC[1];

    printf("\nMensagem DISC recebida: ");
    while (STOP == FALSE) {
      res = read(fd, readMsgDISC, 1);
      if (res == -1 && errno == EINTR) {
        printf("Erro a receber a mensagem DISC do recetor...\n");
        if (num_try < ll.numTransmissions) printf("Nova tentativa...\n");
      }
      else if (res == -1) { 
        printf("Erro a receber a mensagem DISC no número de tentativas permitidas...\n");
        return 1;
      }

      print_0x(readMsgDISC[0]);
      
      stateMachine(readMsgDISC[0], NULL, NULL);

      if (SM.state == SM_STOP || fail) STOP = TRUE;
    }
  } while (num_try < ll.numTransmissions && fail);
  printf("\n\n");
  
  alarm(0);
  if (fail) {
    printf("Emissor: Todas as tentativas de receber DISC foram sem sucesso...\n");
    return 1;
  }

  unsigned char msgUA[SET_UA_SIZE];
  msgUA[0] = FLAG;
  msgUA[1] = A_RecEmi;
  msgUA[2] = C_UA;
  msgUA[3] = BCC(A_RecEmi, C_UA);
  msgUA[4] = FLAG;
  
  res = write(fd, msgUA, SET_UA_SIZE);
  if (res == -1) {
    printf("emissor_DISC(): Erro no envio de mensagem UA\n");
    return 1;
  }
  else {
    printf("Mensagem UA enviada: ");
    for (int i = 0; i < SET_UA_SIZE; i++) print_0x(msgUA[i]);
    printf("\n");
  }

  return fd;
}

int recetor_DISC(int fd) {
  settingUpSM(SV, START, A_EmiRec, C_DISC);

  unsigned char readMsgDISC[1];
  int res;
  volatile int STOP=FALSE;

  printf("\nMensagem DISC recebida: ");
  while (STOP==FALSE) {
    res = read(fd, readMsgDISC, 1);
    if (res == -1) {
      printf("Erro a receber mensagem DISC\n");
      return 1;
    }
    
    print_0x(readMsgDISC[0]);
    
    stateMachine(readMsgDISC[0], NULL, NULL);

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  printf("\n\n");

  // Resposta do recetor
  unsigned char msgDISC[5];
  msgDISC[0] = FLAG;
  msgDISC[1] = A_RecEmi;
  msgDISC[2] = C_DISC;
  msgDISC[3] = BCC(A_RecEmi, C_DISC);
  msgDISC[4] = FLAG;

  res = write(fd, msgDISC, SET_UA_SIZE);
  if (res == -1) {
    printf("recetor_DISC(): Erro no envio de mensagem DISC\n");
    return 1;
  }
  else {
    printf("\nMensagem DISC enviada: ");
    for (int i = 0; i < SET_UA_SIZE; i++) print_0x(msgDISC[i]);
    printf("\n");
  }

  alarm(5);
  STOP = FALSE;
  SM.state = START;
  unsigned char readMsgUA[1];

  printf("\nMensagem UA recebida: ");
  while (STOP==FALSE) {
    res = read(fd, readMsgUA, 1);
    if (res == -1) {
      printf("Erro a receber mensagem UA\n");
      return 1;
    }
    
    print_0x(readMsgUA[0]);
    
    stateMachine(readMsgUA[0], NULL, NULL);

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  printf("\n\n");

  alarm(0);

  return fd;
}

int llclose(int fd) {
  int ret = fd;

  switch (ll.type) {
    case EMISSOR: if (emissor_DISC(fd) != 0) ret = -1; break;
    case RECETOR: if (recetor_DISC(fd) != 0) ret = -1; break;
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