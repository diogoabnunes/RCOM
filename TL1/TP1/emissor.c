/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];
  int i, sum = 0, speed = 0;
    
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

  /*
  TO DO
  - lê uma linha do stdin.
      sugestão: utilizar a função gets() para obter a linha do stdin;
  - determina número de caracteres até ‘\0’;
  - escreve na porta série os caracteres lidos usando a configuração em modo não canónico (incluir
      o ‘\0’ para indicar o fim da transmissão ao receptor);
  - lê da porta série (ver Receptor) a string que deve ter sido reenviada pelo Receptor.
  */
  
  // Leitura
  printf("Input: ");
  fgets(buf, 255, stdin);
  buf[strlen(buf)] = '\0';
  printf("Message written: %s\n", buf);
  size_t size = strlen(buf);

  // Escrita na porta série
  res = write(fd, buf, size);
  printf("%d bytes written\n", res);

  // Leitura do sinal reenviado pelo recetor
  char reply[255];
  res = read(fd, reply, 255);
  printf("Message received: %s\n", reply);

  /*
  for (i = 0; i < 255; i++) {
    buf[i] = 'a';
  }
    
  buf[25] = '\n';
    
  res = write(fd,buf,255);   
  printf("%d bytes written\n", res);
 
  O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
  o indicado no guião 
  */
   
  if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}
