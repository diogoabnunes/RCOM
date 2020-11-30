#include "utils.h"

// ftp://[<user>:<password>@]<host>/<url-path>
// ex.: ftp://diogo:porto@ftp.up.pt/folder/folder2/file.txt

int main(int argc, char *argv[]) {
    printf("RCOM TL2\n\n");

    if (argc != 2) {
        printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }
    struct args URL;
    struct ftp FTP;
    
    if (parseArgs(&URL, argv[1]) != 0) { printf("Error parsing arguments.\n"); return 1; }
    printf("Protocol: %s\n", URL.protocol);
    printf("User: %s\n", URL.user);
    printf("Password: %s\n", URL.password);
    printf("Host: %s\n", URL.host);
    printf("Path: %s\n", URL.path);
    printf("Filename: %s\n\n", URL.filename);

    if (getIPAddress(URL.IP, URL.host) != 0) { printf("Error getting IP address.\n"); return 2; }
    printf("IP Address : %s\n\n", URL.IP);
    
    if (createConnectSocketServer(URL.IP, FTP_PORT) != 0) { printf("Error creating socket server.\n"); return 3; }

    // Can't test the connection because my Linux VPN isn't working :/

    if (receiveConfirmationFromServer() != 0) { printf("Error receiving confirmation from server\n"); return 4; }

    if (login(&FTP, URL.user, URL.password) != 0) { printf("Error in login.\n"); return 5; }

    /*
    printf("TO DO: receiveConfirmationFromServer(...);\n");
    printf("TO DO: login(...);\n");
    printf("TO DO: cd(...);\n");
    printf("TO DO: pasv(...);\n");
    printf("TO DO: retr(...);\n");
    printf("TO DO: download(...);\n");
    printf("TO DO: disconnect(...);\n");*/

    return 0;
}