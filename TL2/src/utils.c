#include "utils.h"

void clearVar(char *var) {
    for (int i = 0; i < strlen(var) - 1; i++) var[i] = ' ';
}

int parseArgs(struct args *URL, char *command) {

    char protocol_expected[6] = "ftp://";
    enum state {PROTOCOL, USER, PASSWORD, HOST, PATH};
    enum state state = PROTOCOL;

    int index = 0;
    for (int i = 0; i <= strlen(command); i++) {

        switch(state) {
            
            case PROTOCOL: 
                if (i == strlen(command) - 1) { printf("Error parsing protocol: reached end of URL.\n"); return 1;}
                if (command[i] == protocol_expected[i] && i < 5) URL->protocol[index] = command[i];
                else if (command[i] == protocol_expected[i] && i == 5) { 
                    strcpy(URL->protocol, "ftp"); 
                    state = USER; index = 0; }
                else { printf("Error parsing protocol. Expected: ftp://\n"); return 1; }
                break;

            case USER: 
                if (i == strlen(command) - 1) { printf("Error parsing username: reached end of URL.\n"); return 2; }
                if (command[i] == ':') {
                    if (i == 6) { printf("Username shouldn't be NULL.\n"); return 2; }  
                    state = PASSWORD; index = 0; }
                else if (command[i] == '@') {
                    strcpy(URL->user, "anonymous");
                    strcpy(URL->password, "");
                    printf("No password given: Logged as anonymous.\n");
                    state = HOST;
                }
                else { URL->user[index] = command[i]; index++; }
                break;

            case PASSWORD: 
                if (i == strlen(command) - 1) { printf("Error parsing password: reached end of URL.\n"); return 3; }
                if (command[i] == '@') {
                    state = HOST; index = 0;
                }
                else { URL->password[index] = command[i]; index++; }
                break;
            
            case HOST: 
                if (i == strlen(command) - 1) { printf("Error parsing host: reached end of URL.\n"); return 4; }
                if (command[i] == '/') {
                    state = PATH; index = 0;
                }
                else { URL->host[index] = command[i]; index++; }
                break;

            case PATH: 
                if (command[i] == '\0') { URL->path[index] = '\0'; break; }
                else { URL->path[index] = command[i]; index++; }
                break;
        }
    }

    index = 0;
    clearVar(URL->filename);
    for (int i = 0; i < strlen(URL->path); i++) {
        if (URL->path[i] == '/' || URL->path[i] == '\\') { index = 0; clearVar(URL->filename); }
        else { URL->filename[index] = URL->path[i]; index++; }
    }
    
    return 0;
}

int getIPAddress(char *ip, char host[]) {
    struct hostent *h;
    if ((h = gethostbyname(host)) == NULL) {  
        herror("gethostbyname");
        return 1;
    }
    strcpy(ip, inet_ntoa(*((struct in_addr *)h->h_addr)));
    return 0;
}

int createConnectSocketServer(char *IP, int port) {
    int	sockfd;
	struct sockaddr_in server_addr;
	
	// Server Address Handling
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP);	// 32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		        // server TCP port must be network byte ordered */
    
	// Open an TCP socket
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("socket()");
        exit(0);
    }

	// Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        return 1;
	}

    return sockfd;
}

int receiveConfirmationFromServer() {
    printf("TO DO\n");
    return 0;
}

// sendToSocket
// receiveFromSocket
// sendCommandAndReply

int login(struct ftp *FTP, char *user, char *password) {
    printf("TO DO\n");
    return 0;
}