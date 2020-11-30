#include "utils.h"

void clearVar(char *var) {
    for (int i = 0; i < strlen(var) - 1; i++) var[i] = ' ';
}

int parseArgs(struct args *URL, char *command) {
    
    // ftp://[<user>:<password>@]<host>/<url-path>
    // ex.: ftp://diogo:porto@ftp.up.pt/folder/folder2/file.txt

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
                else { printf("Error parsing protocol. Expected: ftp\n"); return 1; }
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