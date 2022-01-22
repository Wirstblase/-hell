#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>

/*
	-t -> specify a folder instead of a filename, copies the sourcefile into that folder
	-v -> verbose (prints out source and destination)!
	-i -> if dest file exists -> asks user to overwrite
*/

char * readFrom(char *path);
void binaryCopy(char *sourcePath, char *destPath);
int fileExists(char *path);

int main(int argc, char * argv[]){

	char *sourcePath;
	char *destPath;

	int sourceSelected = 0;
	int interactiveOption = 0, verboseOption = 0, copytoFolder = 0;
	int fileCount = 0;
	int shouldCopy = 0;

	int debugCount=0;
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-i") == 0){
			interactiveOption = 1;
		} else if(strcmp(argv[i],"-v") == 0){
			verboseOption = 1;
		} else if(strcmp(argv[i],"-t")==0){
			copytoFolder = 1;
		} else {fileCount++;}
	}

	char *filelist[fileCount];
	

	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-a") != 0 && strcmp(argv[i],"-v") != 0 && strcmp(argv[i],"-t") != 0 && strcmp(argv[i],"-i") != 0){

			//printf("file\n");
			if(sourceSelected == 0){
				sourcePath = strdup(argv[i]);
				//printf("%s\n",sourcePath);
				sourceSelected++;
			} else {
				debugCount++;
				destPath = strdup(argv[i]);
				//printf("%s\n",destPath);


				if(interactiveOption == 1){
					if(fileExists(destPath)){
						printf("cp:file %s exists, do you want to overwrite it? y/n:",destPath);
					
						char c;
						while((c=getchar())!='\n'){
							if(c!='n'){
								shouldCopy = 1;
								//continue;
							} 
						}
						//shouldCopy = 1;
						
					}
				}

				if(copytoFolder == 1){
					if(fileExists(destPath)){ //checks if file exists, also works in folders apparently
						sprintf(destPath,"%s/%s",destPath,sourcePath); 
						//printf("%s\n",destPath);
						if(fileExists(destPath)){ //now checks if a copy of sourceFile file already exists in that folder
							if(interactiveOption == 0){
								printf("cp: %s already exists!\n",destPath);
							}
						} else {shouldCopy = 1;}
					}
				}
				
				if(verboseOption == 1){
					
					if(fileExists(destPath)){
						if(interactiveOption == 0){
							printf("cp: file %s exists already!\n", destPath);
						}
					} else {
						shouldCopy = 1;
						printf("'%s' -> '%s'\n",sourcePath,destPath);
					}
				}


				if(shouldCopy == 1){
					binaryCopy(sourcePath,destPath);
				}

			}
		
		} 
	}



	return 0;
}

int fileExists(char *path){
	int fd = open(path, O_RDONLY);
	if(fd < 0){
		return 0;
	} else { close(fd); return 1;}

}

void binaryCopy(char *sourcePath, char *destPath){
	mode_t filemode = O_CREAT | O_WRONLY | O_TRUNC;

	struct stat stbf;
	stat(sourcePath, &stbf);
	long size = stbf.st_size;
	int fd = open(sourcePath, O_RDONLY);
	int fd2 = open(destPath, filemode,0644);

	if(fd2 == -1){
		printf("error with the destinationfile");
	}

	char *buf[1];

	for(int i=0;i<size;i++){
		read(fd, &buf[1], 1);
		write(fd2, &buf[1], 1);
	}

	close(fd);

	//return file;
}
