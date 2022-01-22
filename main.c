#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>

int operationMode = 0; //0-standalone 1-server

void execFork(char **argv);

bool countPipes(char *cmd) {
    int pipeCount = 0;
    for (size_t i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == '|') {
            pipeCount++;
        }
    }
    return pipeCount;
}

void runComm(char *cmd) { // checks if the command is custom / system command and runs it
    
    int argc = 0;
    bool customCommand = false;

    char *line;
    line = strdup(cmd);

    char *TOKEN;
    TOKEN = strtok(line, " ");

    while (TOKEN != NULL) {
        argc++;   
        TOKEN = strtok(NULL, " ");
    }

    char **argv;
    argv = malloc((sizeof(char **) * argc) + 1);

    argc = 0;
    line = strdup(cmd);

    TOKEN = strtok(line, " ");
    while (TOKEN != NULL) 
    {
        argv[argc] = strdup(TOKEN);
        argc += 1;
        TOKEN = strtok(NULL, " ");
    }

    if (argv[argc] != NULL)
        argv[argc] = NULL;
    

    int soutbk = dup(1);
    int sinbk = dup(0);
    int fd;
    bool rFlag = false;

    char *toFile;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], ">") == 0) 
        {
            if (argc <= 2) 
            {
                printf("more arguments needed\n");
                exit(0);
            }

            rFlag = true;

            if (argv[i] != NULL && argv[i+1] != NULL) 
            {
                toFile = strdup(argv[i + 1]);
                argv[i] = NULL;
                break;
            } 
            else 
            {
                printf("to what file?\n");
                return;
            }
        }
    }

    if (rFlag) 
    {
        fd = open(toFile, O_WRONLY | O_CREAT | O_TRUNC);
        fchmod(fd, S_IROTH | S_IRGRP | S_IREAD | S_IWUSR | S_IRUSR);
        dup2(fd, STDOUT_FILENO);
    }

    if (strcmp(argv[0],"dirname") == 0 || strcmp(argv[0],"tee") == 0 || strcmp(argv[0],"cp") == 0 || strcmp(argv[0],"cd") == 0) 
        customCommand = true;


    if (strcmp(argv[0], "dirname") == 0) {
        
        pid_t ppid;
        int whatever;

        if ((ppid = fork()) < 0)
        {
            printf("Error forking at line 128.");
            exit(0);
        }

        if(ppid == 0)
        {
            if(execv("dirname", argv)){
                fprintf(stderr,"error with my custom dirname");
                exit(1);
            }
            //return;
        } 
        else 
            if(ppid > 0)  while(wait(&whatever) != ppid);
        
    }


    if (strcmp(argv[0], "tee") == 0) 
    {
        pid_t ppid;
        int whatever;

        if ((ppid = fork()) < 0)
        {
            printf("Error forking at line 128.");
            exit(1);
        }

        if(ppid == 0)
        {
            if(execv("tee", argv)<0){
                fprintf(stderr,"error with the custom tee");
                exit(1);
            }
            //return;
        } 
        else 
            if(ppid > 0)  while(wait(&whatever) != ppid);
    }



    if (strcmp(argv[0], "cp") == 0) 
    {
        pid_t ppid;
        int whatever;

        if ((ppid = fork()) < 0)
        {
            printf("Error forking at line 128.");
            exit(0);
        }

        if(ppid == 0)
        {
            if(execv("cp", argv)<0){
                fprintf(stderr,"error with the custom cp");
                exit(1);
            }
            return;
        } 
        else 
            if(ppid > 0)  while(wait(&whatever) != ppid);
    }

    if (strcmp(argv[0], "cd") == 0){
        //printf("debug: %s", argv[1]);
        chdir(argv[1]);
    }
    
    if (strcmp(argv[0], "[ss]") == 0)
    { //reimplemented in server
        char *cwd = (char *) malloc(sizeof(char) * PATH_MAX);
        getcwd(cwd, PATH_MAX);
        
        char *prompt = (char *) malloc(sizeof(char) * strlen(cwd) * strlen("$hell> "));
        sprintf(prompt, "%s$hell> ", cwd);
        printf(prompt);
    }

    if (customCommand) {
        
    }

    else{
        execFork(argv);
    }

    close(fd);

    dup2(sinbk, STDIN_FILENO);
    dup2(soutbk, STDOUT_FILENO);
    
    close(sinbk);
    close(soutbk);
}

void execFork(char **argv) { //forkception
    
    pid_t pid;
    int status;
    
    if ((pid = fork()) < 0)
        printf("Error forking\n");
    
    if (pid == 0) {
        if (execvp(argv[0], argv)<0)
        {printf("Error: Command not found\n\n");
            exit(2);}
        
    }
    
    else if (pid > 0) {
        while (wait(&status) != pid);
    }
}

void pipeline(char *cmd)
{
    int commandCount = 0;
    int commandCounter = 0;
    char *cmdCopy = strdup(cmd);
    char *TOKEN;
    
    int fd[2], stdin_d = 0;
    pid_t pid;

    TOKEN = strtok(cmdCopy, "|");
    while (TOKEN != NULL) {
        commandCount++;
        TOKEN = strtok(NULL, "|");
    }
    
    char **parsedCommands = malloc(sizeof(char *) * commandCounter + 1);
    cmdCopy = strdup(cmd);

    TOKEN = strtok(cmdCopy, "|");
    while (TOKEN != NULL) 
    {
        parsedCommands[commandCounter] = strdup(TOKEN);
        commandCounter++;
        TOKEN = strtok(NULL, "|");
    }

    if (parsedCommands[commandCounter] != NULL)
        parsedCommands[commandCounter] = NULL;

    int cc = 0;
    while (parsedCommands[cc] != NULL) {
        pipe(fd);
        pid = fork();

        if (pid < 0) 
        {
            printf("ERROR PIPING");
            exit(-1);
        }

        if (pid == 0) {
            dup2(stdin_d, STDIN_FILENO);

            if (parsedCommands[cc+1] != NULL) 
            {
                dup2(fd[1], 1);
            }

            close(fd[0]);
            runComm(parsedCommands[cc]);

            exit(EXIT_SUCCESS);
        }

        if (pid > 0) 
        {
            wait(NULL);
            cc += 1; 

            close(fd[1]);
            stdin_d = fd[0];
        }
    }
}

int main(int argc, char const *argv[]) {
    

    if(operationMode == 0){
        printf("▄▄███▄▄·██╗  ██╗███████╗██╗     ██╗     \n");
        printf("██╔════╝██║  ██║██╔════╝██║     ██║     \n");
        printf("███████╗███████║█████╗  ██║     ██║     \n");
        printf("╚════██║██╔══██║██╔══╝  ██║     ██║     \n");
        printf("███████║██║  ██║███████╗███████╗███████╗\n");
        printf("╚═▀▀▀══╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n");
        printf("            version 3.1  ಠ_ಠ            \n");

    }

    int run = 1;
    while (run > 0) { //if in server mode it runs the program only once, if in standalone mode it runs indefinitely
                    //because server has its own runloop 
        
        if(operationMode == 0) //0-standalone 1-server
            run++;
        
        char *cwd = (char *) malloc(sizeof(char) * PATH_MAX);
        getcwd(cwd, PATH_MAX);

        char *prompt = (char *) malloc(sizeof(char) * strlen(cwd) * strlen("$hell> "));
        sprintf(prompt, "%s$hell> ", cwd);

        char *cmd = readline(prompt);
        add_history(cmd);

        if (strlen(cmd) == 0) {
            printf("illegal command\n");
            //continue;
        }

        if (strcmp(cmd, "(cl0se)") == 0){
            operationMode = 1;
            run = 0; //stops the runloop (not tested for server)
        } //break;
    
        int a = countPipes(cmd);
        if(a > 0)
            pipeline(cmd); //if there are pipe symbols it sends the string over to the p i p e l i n e
        else
            runComm(cmd); //if there are no pipe symbols it runs command directly


        free(cmd);
        free(cwd);
        
        run--;
    }

    return 0;
}
