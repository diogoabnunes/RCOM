#ifndef APP_H
#define APP_H

struct applicationLayer {
  int type;
  char port[255];
  char filename[255];
  char destination[255];

  char file[255];
  off_t filesize;
} app;

#endif // APP_H