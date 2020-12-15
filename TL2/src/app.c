#include "utils.h"

int main(int argc, char *argv[]) {
	//printf("RCOM TL2\n\n");

	if (argc != 2) {
		printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
		exit(1);
	}

	struct args URL;
	if (parseArgs(&URL, argv[1]) != 0) { printf("Error parsing arguments.\n"); return 1; }
	/*printf("Protocol: %s\n", URL.protocol);
	printf("User: %s\n", URL.user);
	printf("Password: %s\n", URL.password);
	printf("Host: %s\n", URL.host);*/
	printf("Path: %s\n", URL.path);
	printf("Filename: %s\n\n", URL.filename);

	if (getIPAddress(URL.IP, URL.host) != 0) { printf("Error getting IP address.\n"); return 2; }
	/*printf("IP Address : %s\n\n", URL.IP);*/

	struct ftp ftp;
	ftp.fd = openConnectSocketServer(URL.IP, FTP_PORT);
	if (ftp.fd == -1) { printf("Error opening TCP socket.\n"); return 3; }
	if (ftp.fd == -2) { printf("Error connecting to the server.\n"); return 4; }
    //else printf("Connection estabilished in port %d.\n", port);

	ftp.file = fdopen(ftp.fd, "r");
	receiving(ftp.file);

	// Sending username
	char userCommand[MAX];
	sprintf(userCommand, "user %s\r\n", URL.user);
	sending(ftp.fd, userCommand);
	receiving(ftp.file); // 331 Please specify the password.

	// Sending password
	char passwordCommand[MAX];
	sprintf(passwordCommand, "pass %s\r\n", URL.password);
	sending(ftp.fd, passwordCommand);
	receiving(ftp.file); // 230 Login successful.

	// Sending pasv
	char pasvCommand[MAX], ip[16]; int port;
	sprintf(pasvCommand, "pasv\r\n");
	sending(ftp.fd, pasvCommand);
	receivingPasvCommand(ftp.file, ip, &port); // 227 Entering Passive Mode (193,137,29,15,port1,port2). -> port = port1 * 256 + port2
	if (strcmp(ip, URL.IP) != 0) {printf("Error parsing pasv.\n"); return 5; }

	// Connecting to port server, waiting for connection
	ftp.data_fd = openConnectSocketServer(URL.IP, port);
	if (ftp.data_fd == -1) { printf("Error opening TCP socket.\n"); return 3; }
	if (ftp.data_fd == -2) { printf("Error connecting to the server.\n"); return 4; }
    //else printf("Connection estabilished in port %d.\n", port);

	char retrCommand[MAX];
	sprintf(retrCommand, "retr %s\r\n", URL.path);
	sending(ftp.fd, retrCommand);
	receiving(ftp.file); // 150 Opening BINARY mode data connection for pub/apache/HEADER.html (770 bytes).

	if (downloadFile(ftp.data_fd, URL.filename) != 0) { printf("Error transfering file.\n"); return 5; }

	receiving(ftp.file); // 226 Transfer complete.

	return 0;
}