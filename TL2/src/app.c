#include "utils.h"

int main(int argc, char *argv[]) {
	printf("RCOM TL2\n\n");

	if (argc != 2) {
		printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
		exit(1);
	}
	struct args URL;

	if (parseArgs(&URL, argv[1]) != 0) { printf("Error parsing arguments.\n"); return 1; }
	printf("Protocol: %s\n", URL.protocol);
	printf("User: %s\n", URL.user);
	printf("Password: %s\n", URL.password);
	printf("Host: %s\n", URL.host);
	printf("Path: %s\n", URL.path);
	printf("Filename: %s\n\n", URL.filename);

	if (getIPAddress(URL.IP, URL.host) != 0) { printf("Error getting IP address.\n"); return 2; }
	printf("IP Address : %s\n\n", URL.IP);

	struct ftp ftp;
	ftp.file_fd = openConnectSocketServer(URL.IP, FTP_PORT);
	if (ftp.file_fd == -1) { printf("Error opening TCP socket.\n"); return 3; }
	if (ftp.file_fd == -2) { printf("Error connecting to the server.\n"); return 4; }

	ftp.file = fdopen(ftp.file_fd, "r");
	receiving(ftp.file);

	// Sending username
	char userCommand[MAX];
	sprintf(userCommand, "user %s\n", URL.user);
	sending(ftp.file_fd, userCommand);
	receiving(ftp.file);

	char passwordCommand[MAX];
	sprintf(passwordCommand, "pass %s\n", URL.password);
	sending(ftp.file_fd, passwordCommand);
	receiving(ftp.file);

	char pasvCommand[MAX], ip[16]; int port;
	sprintf(pasvCommand, "pasv\n");
	sending(ftp.file_fd, pasvCommand);
	receivingPasvCommand(ftp.file, ip, &port);

	if (strcmp(ip, URL.IP) != 0) {printf("Error parsing pasv.\n"); return 5; }

	ftp.data_fd = openConnectSocketServer(URL.IP, port);
	if (ftp.data_fd == -1) { printf("Error opening TCP socket.\n"); return 3; }
	if (ftp.data_fd == -2) { printf("Error connecting to the server.\n"); return 4; }

	
/*
	printf("TO DO: retr(...);\n");
	printf("TO DO: download(...);\n");
	printf("TO DO: disconnect(...);\n");*/

	return 0;
}