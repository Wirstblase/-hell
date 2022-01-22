#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

char * readFromSTDIN(int *size);

int main(int argc, char* argv[]){

	bool shouldAppend = false; // -a

	int fileCount = 0;

	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-a") == 0){
			shouldAppend = true;
		} else {
			fileCount++;
		}
	}

	mode_t filemode = O_CREAT | O_WRONLY | O_TRUNC;

	if(shouldAppend){
		filemode = O_CREAT | O_APPEND | O_WRONLY;
	}


	char *fileList[fileCount];
	fileCount = 0;
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-a") != 0){
			fileList[fileCount++] = strdup(argv[i]);
		} 
	}

	int size;
	char * content = readFromSTDIN(&size);

	for(int i=0;i<fileCount;i++){
		int fd = open(fileList[i],filemode,0644); 
		if(fd < 0){
			fprintf(stderr,"tee: could not open file '%s': ",fileList[i]);
			perror(NULL);
			continue;
		}
		for(int j=0;j<size;j++){
			if(write(fd, &content[j] , 1) < 0){
			fprintf(stderr,"tee: could not write to file '%s': ",fileList[i]);
			perror(NULL);
			continue;	
			}
		}

		if(close(fd) < 0){
			fprintf(stderr,"tee: could not close file '%s': ",fileList[i]);
			perror(NULL);
			continue;
		}
	}


	for(int i=0;i<size;i++){
		write(STDOUT_FILENO, &content[i] , 1);
	}

	free(content);
	for(int i=0;i<fileCount;i++){
		free(fileList[i]);
	}


	return 0;
}

char * readFromSTDIN(int *size){
	int i = 0;
	char* content = (char*) malloc (1 * sizeof(char));
	char c;
	if (content != NULL){
	content[0] = '\0';
	while ((c = getchar()) != EOF)
	{

	content[i++]=c;
	content = (char*) realloc (content, (i+1) * sizeof(char));
   
	}
	content[i] = '\0';

  
}

	*size = i;
	return content;

}
