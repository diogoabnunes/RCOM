#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100
#define FTP_PORT 21

struct args {
    char protocol[5];

    char user[MAX];
    char password[MAX];

    char host[MAX];

    char path[MAX];
    char filename[MAX];
} URL;

struct ftp {
    int fd_control_socket;
    int fd_data_socket;
};

void clearVar(char *var);

int parseArgs(struct args *URL, char *command);

#endif // UTILS_H