#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include<arpa/inet.h>

#define MAX 256
#define FTP_PORT 21

struct args {
    char protocol[5];
    char user[MAX];
    char password[MAX];
    char host[MAX];
    char path[MAX];
    char filename[MAX];
    char IP[MAX];
} URL;

struct ftp {
    int fd_control_socket;
    int fd_data_socket;
};

void clearVar(char *var);

int parseArgs(struct args *URL, char *command);

int getIPAddress(char *ip, char host[]);

int createConnectSocketServer(char *IP, int port);

int receiveConfirmationFromServer();

int login(struct ftp *FTP, char *user, char *password);

#endif // UTILS_H