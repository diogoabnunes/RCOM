#include "utils.h"

int parseArgs(struct args *URL, char *command) {

    char* ftp = strtok(command, "/");   // ftp:
    char* remaining = strtok(NULL, "/");  // diogo:feup@ftp.up.pt
    URL->path = strtok(NULL, "");      // path/to/destination/file.txt

    if (strcmp(ftp, "ftp:") != 0) { printf("Error parsing protocol: Expected ftp.\n"); return 1; }
    ftp[strlen(ftp) - 1] = '\0';
    URL->protocol = ftp;
    URL->user = strtok(remaining, ":");
    URL->password = strtok(NULL, "@");
    
    if (URL->password == NULL)
    {
        URL->user = "anonymous";
        URL->password = "1234";
        URL->host = remaining;
    }
    else URL->host = strtok(NULL, "");

    parseFilename(URL);

    return 0;
}

int parseFilename(struct args *URL) {
  char fullpath[256];
  strcpy(fullpath, URL->path);
  char* token = strtok(fullpath, "/");
  while( token != NULL ) {
    strcpy(URL->filename, token);
    token = strtok(NULL, "/");
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

int openConnectSocketServer(char *IP, int port) {
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
        return -1;
    }

	// Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        return -2;
	}

    return sockfd;
}

int sending(int sockfd, char *command) {
    printf("%s", command);
    int sent = write(sockfd, command, strlen(command));

    if (sent == 0) { printf("Connection closed.\n"); return 1; }
    else if (sent == -1) { printf("Error sending command.\n"); return 2; }

    return 0;
}

char* receiving(FILE * sockfile) {
    char *buf;
	size_t bytes = 0;

	// Sending username
	while (1) {
		getline(&buf, &bytes, sockfile);
		printf("%s", buf);
		if (buf[3] == ' ') break;
	}

    return buf;
}

char* receivingPasvCommand(FILE* sockfile, char* serverIP, int *serverPort) {
    char *buf = receiving(sockfile);

    strtok(buf, "(");

    char* IP[4];
    for (int i = 0; i < 4; i++) {
        IP[i] = strtok(NULL, ",");
    }
    
    sprintf(serverIP, "%s.%s.%s.%s", IP[0], IP[1], IP[2], IP[3]);

    char *port1 = strtok(NULL, ",");
    char *port2 = strtok(NULL, ")");
    *serverPort = atoi(port1)*256 + atoi(port2);

    return buf;
}

int downloadFile(int sockfd, char *filename) {
    
    int file_fd = open(filename, O_WRONLY | O_CREAT, 0777);
    if (file_fd < 0) { printf("Error creating file.\n"); return 1; }

    int bytes; char buf[1];
    do {
        bytes = read(sockfd, buf, 1);
        //printf("%s", buf);
        write(file_fd, buf, bytes);
    } while (bytes != 0);

    return 0;
}