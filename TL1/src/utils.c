#include "utils.h"

static struct sigaction old; // Para usar no restauro do SIGALRM

int baudrate_number(int b) {
    switch(b) {
        case B0: return 0;
        case B50: return 50;
        case B75: return 75;
        case B110: return 110;
        case B134: return 134;
        case B150: return 150;
        case B200: return 200;
        case B300: return 300;
        case B600: return 600;
        case B1200: return 1200;
        case B1800: return 1800;
        case B2400: return 2400;
        case B4800: return 4800;
        case B9600: return 9600;
        case B19200: return 19200;
        case B38400: return 38400;
        default: return -1;
    }
}

void print_0x(unsigned char a) {
  if (a == 0) printf("0x00 ");
  else        printf("%#.2x " , a);
}

void atende() {
  if (SM.state != SM_STOP) {
    fail = TRUE;
    printf("Alarm!\n");
    return;
  }
}

int setting_alarm_handler() {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = atende;
  sa.sa_flags = 0;
  if (sigaction(SIGALRM, &sa, &old) < 0) {
    printf("Erro no sigaction...\n");
    return -1;
  }
  return 0;
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
  int try = 0;
  volatile int STOP = FALSE;

  do {
    try++;
    int res = write(fd, buf, bufsize);
    if (res == -1) {
      printf("Erro a enviar Trama I para o buffer no ciclo_write()...\n");
      return -1;
    }

    alarm(ll.timeout);
    SM.state = START;
    fail = FALSE;
    unsigned char readBuf[1];

    while (STOP == FALSE) {
      res = read(fd, readBuf, 1);

      if (res == -1 && errno == EINTR) {
        printf("Erro a receber RR do recetor no ciclo_write()...\n");
        if (try < ll.numTransmissions) printf("Nova tentativa...\n");
        else {  
          printf("Excedeu o número de tentativas...\n");
          return -1;
        }
        break;
      }
      else if (res == -1) {
          printf("Erro a receber RR do buffer no ciclo_write()...\n");
          return -1;
      }

      if (stateMachine(readBuf[0], NULL, NULL) < 0) {
        printf("Erro a receber RR ou REJ no ciclo_write()...\n");
        fail = TRUE;
        alarm(0);
        break;
      }

      if (SM.state == SM_STOP || fail) STOP = TRUE;
    }
  } while (try < ll.numTransmissions && fail);

  alarm(0);
  return 0;
}

int ciclo_read(int fd, unsigned char *C, unsigned char **dataBuf, int *size) {
  volatile int STOP = FALSE;
  unsigned char buf[1];

  while (STOP == FALSE) {
    int res = read(fd, buf, 1);
    if (res == -1) {
      printf("Erro a receber trama I do buffer no ciclo_read()...\n");
      return -1;
    }

    int smRead = stateMachine(buf[0], dataBuf, size);
    if (smRead == -1) {
      *C = C_REJ(ll.sequenceNumber);
      printf("Erro no BCC... no ciclo_read()...\n");
      return -1;
    }
    else if (smRead == -2) {
      *C = C_RR(ll.sequenceNumber);
      printf("Número de sequência errado em C no ciclo_read()...\n");
      return -2;
    }
    *C = C_RR(ll.sequenceNumber);

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  return 0;
}

int emissor_SET(int fd) {
  unsigned char buf[SET_UA_DISC_SIZE];
  buf[0] = FLAG;
  buf[1] = A_EmiRec;
  buf[2] = C_SET;
  buf[3] = XOR(A_EmiRec, C_SET);
  buf[4] = FLAG;

  settingUpSM(SV, START, A_RecEmi, C_UA);

  int res, num_try = 0;
  volatile int STOP=FALSE;

  do {
    num_try++;
    res = write(fd, buf, SET_UA_DISC_SIZE);
    if (res == -1) {
      printf("Erro no envio de mensagem SET...\n");
      return 1;
    }
    else {
      printf("Mensagem SET enviada: ");
      for (int i = 0; i < SET_UA_DISC_SIZE; i++) print_0x(buf[i]);
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
        else {
          printf("Excedeu o número de tentativas...");
          return -1;
        }
        break;
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
  unsigned char reply[SET_UA_DISC_SIZE];
  reply[0] = FLAG;
  reply[1] = A_RecEmi;
  reply[2] = C_UA;
  reply[3] = XOR(A_RecEmi, C_UA);
  reply[4] = FLAG;

  res = write(fd, reply, SET_UA_DISC_SIZE);
  if (res == -1) {
    printf("Erro no envio de mensagem UA\n");
    return 1;
  }
  else { 
    printf("Mensagem UA enviada: ");
    for (int i = 0; i < SET_UA_DISC_SIZE; i++) print_0x(reply[i]);
    printf("\n\n");
  }

  return fd;
}

int emissor_DISC(int fd) {
  unsigned char msgDISC[SET_UA_DISC_SIZE];
  msgDISC[0] = FLAG;
  msgDISC[1] = A_EmiRec;
  msgDISC[2] = C_DISC;
  msgDISC[3] = XOR(A_EmiRec, C_DISC);
  msgDISC[4] = FLAG;

  settingUpSM(SV, START, A_RecEmi, C_DISC);

  int res, num_try = 0;
  volatile int STOP=FALSE;

  do {
    num_try++;
    res = write(fd, msgDISC, SET_UA_DISC_SIZE);
    if (res == -1) {
      printf("Erro no envio de mensagem DISC\n");
      return -1;
    }
    else { 
      printf("Mensagem DISC enviada: ");
      for (int i = 0; i < SET_UA_DISC_SIZE; i++) print_0x(msgDISC[i]);
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
        else {
          printf("Excedeu o número de tentativas...\n");
          return -1;
        }
        break;
      }
      else if (res == -1) { 
        printf("Erro a receber a mensagem DISC do buffer...\n");
        return -1;
      }
      print_0x(readBuf[0]);

      stateMachine(readBuf[0], NULL, NULL);

      if (SM.state == SM_STOP || fail) STOP = TRUE;
    }
  } while (num_try < ll.numTransmissions && fail);
  printf("\n");

  if (fail) {
    printf("Todas as tentativas de receber DISC foram sem sucesso...\n");
    return -1;
  }
  
  alarm(0);

  unsigned char msgUA[SET_UA_DISC_SIZE];
  msgUA[0] = FLAG;
  msgUA[1] = A_RecEmi;
  msgUA[2] = C_UA;
  msgUA[3] = XOR(A_RecEmi, C_UA);
  msgUA[4] = FLAG;
  
  res = write(fd, msgUA, SET_UA_DISC_SIZE);
  if (res == -1) {
    printf("emissor_DISC(): Erro no envio de mensagem UA\n");
    return -1;
  }
  else { 
    printf("Mensagem UA enviada: ");
    for (int i = 0; i < SET_UA_DISC_SIZE; i++) print_0x(msgUA[i]);
    printf("\n\n");
  }

  sleep(3);

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
  unsigned char reply[SET_UA_DISC_SIZE];
  reply[0] = FLAG;
  reply[1] = A_RecEmi;
  reply[2] = C_DISC;
  reply[3] = XOR(A_RecEmi, C_DISC);
  reply[4] = FLAG;

  res = write(fd, reply, SET_UA_DISC_SIZE);
  if (res == -1) {
    printf("Erro no envio de mensagem DISC\n");
    return -1;
  }
  else {
    printf("\nMensagem DISC enviada: ");
    for (int i = 0; i < SET_UA_DISC_SIZE; i++) print_0x(reply[i]); 
    printf("\n");
  }

  settingUpSM(SV, START, A_RecEmi, C_UA);
  STOP = FALSE;
  unsigned char readMsgUA[1];
  alarm(ll.timeout);

  printf("Mensagem UA recebida: ");
  while (STOP==FALSE) {
    res = read(fd, readBuf, 1);
    if (res == -1) {
      printf("Erro a receber mensagem UA\n");
      return -1;
    }
    print_0x(readBuf[0]);

    stateMachine(readBuf[0], NULL, NULL);

    if (SM.state == SM_STOP) STOP = TRUE;
  }
  printf("\n\n");

  alarm(0);

  sleep(3);

  return fd;
}