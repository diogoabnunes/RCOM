#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#define MAX 256
#define FTP_PORT 21

struct args {
    char *protocol;
    char *user;
    char *password;
    char *host;
    char *path;
    char *filename;
    char *IP;
};

struct ftp {
    FILE *file;
    int file_fd;
    int data_fd;
};

void clearVar(char *var);

int parseArgs(struct args *URL, char *command);

int parseFilename(struct args *URL);

int getIPAddress(char *ip, char host[]);

int openConnectSocketServer(char *IP, int port);

int sending(int sockfd, char *command);

char* receiving(FILE * sockfile);

char* receivingPasvCommand(FILE* sockfile, char* serverIP, int *serverPort);

int downloadFile();

#endif // UTILS_H