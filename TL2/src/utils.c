#include "utils.h"

int parseArgs(struct args *args, char *command) {
    printf("parseArgs()!\n");
    
    // ftp://[<user>:<password>@]<host>/<url-path>
    // ex.: ftp://diogo:porto@ftp.up.pt/path

    char *token = strtok(command, ":");
    if (strcmp(token, "ftp") != 0) {
        printf("Protocol name not expected: expected ftp...\n");
        return 1;
    }
    else strcpy(args->protocol, token);

    token = strtok(NULL, "\0");
    char remaining[MAX];
    strcpy(remaining, token);

    char aux[MAX];
    strcpy(aux, remaining);
    token = strtok(aux, ":");

    if (token[0] != '/' || token[1] != '/' || strlen(token) <= 2) {
        printf("Error parsing user name... Make sure you have // before username...\n");
        return 1;
    }
    else if (strcmp(token, remaining) == 0) { // no password -> anonymous
        strcpy(args->user, "anonymous");
        strcpy(args->password, "");

        char aux2[MAX];
        strcpy(aux2, &remaining[2]);
        strcpy(remaining, aux2); // remaining beginning on @
    }
    else { // user + password
        strcpy(args->user, &token[2]);

        char *token2 = &token[strlen(token)-1];
        token = strtok(NULL, "@");
        if (token == NULL || strlen(token) == 0) { // error when user:<nopass>@...
            printf("Error parsing password... It shouldn't be null or with length 0...\n");
            return 1;
        }
        strcpy(args->password, token);

        token = strtok(NULL, "\0");
        strcpy(remaining, token);
    }

    printf("utils:\n");
    printf("User: %s\n", args->user);
    printf("Password: %s\n", args->password);
    /*printf("Host: %s\n", args->host);
    printf("Path: %s\n", args->path);
    printf("Filename: %s\n", args->filename);*/
    
    return 0;
}