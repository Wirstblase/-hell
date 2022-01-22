#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<unistd.h>	//write
#include<sys/syscall.h>
#include<stdlib.h>
#include<limits.h>

int slashCount(char * thing){
    int k=0;
    for(int i=0;i<strlen(thing);i++){
        if(thing[i] == '/')
            k++;
    }
    return k;
}

int main(int argc, char * argv[]){
	
    char path[PATH_MAX];
    strcpy(path,argv[1]);
    int hasSlash=0;
    
    if(path[0]=='/'){
        hasSlash=1;
    } else {
        //printf("n are slash\n");
    }
    
    int slashC = slashCount(path),j=0;
    
    if(hasSlash == 1){
        for(int i=0;i<strlen(path);i++){
        if(path[i] == '/')
            j++;
        if(j == slashC)
            path[i] = '\0';
     
        }
        printf("%s\n",path);
        
    } else printf(".\n");
    
    
    return 0;
}
