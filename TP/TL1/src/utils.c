#include "utils.h"

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

int llinit(int *fd, char *port) {

    *fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); exit(-1); }

    strcpy(ll.port, port);
    ll.baudRate = BAUDRATE;
    ll.sequenceNumber = 0x00;
    ll.timeout = TIMEOUT;
    ll.numTransmissions = TRIES;

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

int ciclo_write(int fd, unsigned char *buf, int bufsize) {
    int num_try = 0, res;
    volatile int STOP = FALSE;

    // Envio de Trama I
    do {
        num_try++;
        res = write(fd, buf, bufsize);
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
          printf("Erro a receber RR do buffer\n");
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
  return 0;
}

int ciclo_read(int fd, unsigned char *C, unsigned char **dataBuf, int *size) {
  volatile int STOP = FALSE;
  unsigned char buf[1];
  int res;

  while (STOP == FALSE) {
    res = read(fd, buf, 1);
    if (res == -1) {
      printf("Erro a receber trama I...\n");
      return -1;
    }

    int smRead = stateMachine(buf[0], dataBuf, size);
    if (smRead == -1) {
      *C = C_REJ(ll.sequenceNumber);
      printf("Erro no BCC...\n");
      return -1;
    }
    else if (smRead == -2) {
      *C = C_RR(ll.sequenceNumber);
      printf("Número de sequência errado em no byte C...\n");
      break;
    }
    *C = C_RR(ll.sequenceNumber);

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  return 0;
}

int emissor_SET(int fd) {
  unsigned char buf[SET_UA_SIZE];
  buf[0] = FLAG;
  buf[1] = A_EmiRec;
  buf[2] = C_SET;
  buf[3] = BCC(A_EmiRec, C_SET);
  buf[4] = FLAG;

  settingUpSM(SV, START, A_RecEmi, C_UA);

  int res, num_try = 0;
  volatile int STOP=FALSE;

  do {
    num_try++;
    res = write(fd, buf, SET_UA_SIZE);
    if (res == -1) {
      printf("Erro no envio de mensagem SET\n");
      return 1;
    }
    else {
      printf("Mensagem SET enviada: ");
      for (int i = 0; i < SET_UA_SIZE; i++) print_0x(buf[i]);
      printf("\n");
    }

    alarm(ll.timeout);
    SM.state = START;
    fail = FALSE;
    unsigned char readBuf[1];

    printf("Mensagem UA recebida: ");
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);
      if (res == -1 && errno == EINTR) {
        printf("Erro a receber a mensagem UA do recetor...\n");
        if (num_try < ll.numTransmissions) printf("Nova tentativa...\n");
      }
      else if (res == -1) {
        printf("Erro a receber a mensagem UA do buffer...\n");
        return 1;
      }
      print_0x(readBuf[0]);
      
      stateMachine(readBuf[0], NULL, NULL); // **buf and *size not needed here

      if (SM.state == SM_STOP || fail) STOP = TRUE;
    }
  } while (num_try < ll.numTransmissions && fail);
  printf("\n\n");

  if (fail) {
    printf("Todas as tentativas de receber UA foram sem sucesso...\n");
    return 1;
  }

  alarm(0);

  return fd;
}

int recetor_UA(int fd) {
  settingUpSM(SV, START, A_EmiRec, C_SET);

  unsigned char buf[1];
  int res;
  volatile int STOP=FALSE;

  printf("Mensagem SET recebida: ");
  while (STOP==FALSE) {
    res = read(fd, buf, 1);
    if (res == -1) {
      printf("Erro a receber mensagem SET\n");
      return 1;
    }
    print_0x(buf[0]);

    stateMachine(buf[0], NULL, NULL); // **buf and *size not needed here

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  printf("\n");

  // Resposta do recetor
  unsigned char reply[SET_UA_SIZE];
  reply[0] = FLAG;
  reply[1] = A_RecEmi;
  reply[2] = C_UA;
  reply[3] = BCC(A_RecEmi, C_UA);
  reply[4] = FLAG;

  res = write(fd, reply, SET_UA_SIZE);
  if (res == -1) {
    printf("Erro no envio de mensagem UA\n");
    return 1;
  }
  else { 
    printf("Mensagem UA enviada: ");
    for (int i = 0; i < SET_UA_SIZE; i++) print_0x(reply[i]);
    printf("\n\n");
  }

  return fd;
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
      printf("Erro no envio de mensagem DISC\n");
      return -1;
    }
    else { 
      printf("Mensagem DISC enviada: ");
      for (int i = 0; i < SET_UA_SIZE; i++) print_0x(msgDISC[i]);
      printf("\n");
    }

    alarm(ll.timeout);
    SM.state = START;
    fail = FALSE;
    unsigned char readBuf[1];

    printf("Mensagem DISC recebida: ");
    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);
      if (res == -1 && errno == EINTR) {
        printf("Erro a receber a mensagem DISC do recetor...\n");
        if (num_try < ll.numTransmissions) printf("Nova tentativa...\n");
      }
      else if (res == -1) { 
        printf("Erro a receber a mensagem DISC no número de tentativas permitidas...\n");
        return -1;
      }
      print_0x(readBuf[0]);

      stateMachine(readBuf[0], NULL, NULL); // **buf and *size not needed here

      if (SM.state == SM_STOP || fail) STOP = TRUE;
    }
  } while (num_try < ll.numTransmissions && fail);
  printf("\n");

  if (fail) {
    printf("Todas as tentativas de receber DISC foram sem sucesso...\n");
    return -1;
  }
  
  alarm(0);

  unsigned char msgUA[SET_UA_SIZE];
  msgUA[0] = FLAG;
  msgUA[1] = A_EmiRec;
  msgUA[2] = C_UA;
  msgUA[3] = BCC(A_EmiRec, C_UA);
  msgUA[4] = FLAG;
  
  res = write(fd, msgUA, SET_UA_SIZE);
  if (res == -1) {
    printf("emissor_DISC(): Erro no envio de mensagem UA\n");
    return -1;
  }
  else { 
    printf("Mensagem UA enviada: ");
    for (int i = 0; i < SET_UA_SIZE; i++) print_0x(msgUA[i]);
    printf("\n\n");
  }

  return fd;
}

int recetor_DISC(int fd) {
  settingUpSM(SV, START, A_EmiRec, C_DISC);

  unsigned char readBuf[1];
  int res;
  volatile int STOP=FALSE;

  printf("Mensagem DISC recebida: ");
  while (STOP==FALSE) {
    res = read(fd, readBuf, 1);
    if (res == -1) {
      printf("Erro a receber mensagem DISC\n");
      return -1;
    }
    print_0x(readBuf[0]);

    stateMachine(readBuf[0], NULL, NULL);

    if (SM.state == SM_STOP) STOP = TRUE;
  }

  // Resposta do recetor
  unsigned char reply[SET_UA_SIZE];
  reply[0] = FLAG;
  reply[1] = A_RecEmi;
  reply[2] = C_DISC;
  reply[3] = BCC(A_RecEmi, C_DISC);
  reply[4] = FLAG;

  res = write(fd, reply, SET_UA_SIZE);
  if (res == -1) {
    printf("Erro no envio de mensagem DISC\n");
    return -1;
  }
  else {
    printf("\nMensagem DISC enviada: ");
    for (int i = 0; i < SET_UA_SIZE; i++) print_0x(reply[i]); 
    printf("\n");
  }

  settingUpSM(SV, START, A_EmiRec, C_UA);
  STOP = FALSE;
  unsigned char readMsgUA[1];

  printf("Mensagem UA recebida: ");
  while (STOP==FALSE) {
    res = read(fd, readMsgUA, 1);
    if (res == -1) {
      printf("Erro a receber mensagem UA\n");
      return -1;
    }
    print_0x(readMsgUA[0]);

    stateMachine(readMsgUA[0], NULL, NULL);

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  alarm(0);
  printf("\n\n");

  return fd;
}