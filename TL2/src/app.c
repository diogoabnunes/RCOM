#include "utils.h"

int main(int argc, char *argv[]) {
    printf("RCOM TL2\n\n");

    if (argc != 2) {
        printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }
    struct args URL;
    
    if (parseArgs(&URL, argv[1]) != 0) return 1;
    printf("Protocol: %s\n", URL.protocol);
    printf("User: %s\n", URL.user);
    printf("Password: %s\n", URL.password);
    printf("Host: %s\n", URL.host);
    printf("Path: %s\n", URL.path);
    printf("Filename: %s\n\n", URL.filename);

    if (getIPAddress(URL.IP, URL.host) != 0) return 1;
    printf("IP Address : %s\n", URL.IP);
    
    
    
    /*
    printf("TO DO: createConnectSocketServer(...);\n");
    printf("TO DO: receiveConfirmationFromServer(...);\n");
    printf("TO DO: login(...);\n");
    printf("TO DO: cd(...);\n");
    printf("TO DO: pasv(...);\n");
    printf("TO DO: retr(...);\n");
    printf("TO DO: download(...);\n");
    printf("TO DO: disconnect(...);\n");*/

    return 0;
}