#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main(int argc, char *argv[]) {
    printf("RCOM TL2\n\n");

    if (argc != 2) {
        printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    struct args args;
    if (parseArgs(&args, argv[1]) != 0) return 1;

    printf("main:\n");
    printf("User: %s\n", args.user);
    printf("Password: %s\n", args.password);
    /*printf("Host: %s\n", args.host);
    printf("Path: %s\n", args.path);
    printf("Filename: %s\n\n\n", args.filename);*/
    printf("TO DO: getIP(...);\n");
    printf("TO DO: createConnectSocketServer(...);\n");
    printf("TO DO: receiveConfirmationFromServer(...);\n");
    printf("TO DO: login(...);\n");
    printf("TO DO: cd(...);\n");
    printf("TO DO: pasv(...);\n");
    printf("TO DO: retr(...);\n");
    printf("TO DO: download(...);\n");
    printf("TO DO: disconnect(...);\n");

    return 0;
}