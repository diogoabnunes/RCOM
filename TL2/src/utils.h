#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>

#define MAX 100
#define FTP_PORT 21

struct args {
    char protocol[MAX];

    char user[MAX];
    char password[MAX];

    char host[MAX];

    char path[MAX];
    char filename[MAX];
};

struct ftp {
    int fd_control_socket;
    int fd_data_socket;
};

int parseArgs(struct args *args, char *command);

#endif // UTILS_H