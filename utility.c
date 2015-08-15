/** @file utility.c
 *  @brief Implementation of some helpful functions
 *
 *
 *  @author Bishal Santra
 */
#include "utility.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>


char* trim(char* str){
	if(str == NULL){
		return "";
	}
	if(strlen(str) == 0){
		return "";
	}
	char *end;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
	return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end+1) = 0;

	return str;
}

void checkRedirect(char* arg){
	STDOUT_BACKUP = 11;
	STDIN_BACKUP = 10;
	char *original = arg;
	
	arg = trim(arg);
	// printf("Before redirection: %s\n", arg);
	char* rDirLoc;
	if((rDirLoc = strstr(original, ">")) != NULL){
		
		//Look for redirection operators
		if(rDirLoc[1] != '>'){
			arg = rDirLoc + 1;
			if(rDirLoc == '\0'){
				fprintf(stderr, "Enter filename\n");
				exit(EXIT_FAILURE);
			}
			arg = trim(arg);
			char filename[256];

			char *start = arg;
			printf("%s\n", start);
			//Get the filename
			strcpy(filename, start);
			char *end = start;
			while(end[0] != ' ' && end[0] != '\0'){
				end++;
			}
			if(start >= end){
				fprintf(stderr, "Enter filename\n");
				exit(EXIT_FAILURE);
			}
			if(end[0] == ' '){
				strcpy(rDirLoc, filename + (end - start + 1)); 
			}else if(end[0] == '\0'){
				rDirLoc[0] = '\0';
			}
			filename[end - start + 1] = '\0';
			// printf("Write to file [%s].\n", filename);

			int fd = open(filename,O_WRONLY|O_CREAT,S_IRWXU);
		    if(fd != -1){
		    	//Backup stdout to a seperate index in fd-table
		    	dup2(1,STDOUT_BACKUP);
		    	STDOUT_CHANGED = true;
		      dup2(fd, 1); //Replace stdout
		      close(fd);
		      // close(1);
		    }else{
		      perror("open");
		    }
		}
	}


	if((rDirLoc = strstr(original, "<")) != NULL){
		//arg = strtok(arg," ");
		arg = rDirLoc + 1;
		if(rDirLoc == '\0'){
			fprintf(stderr, "Enter filename\n");
			exit(EXIT_FAILURE);
		}
		arg = trim(arg);
		char filename[256];

		char *start = arg;
		printf("%s\n", start);
		strcpy(filename, start);
		char *end = start;
		while(end[0] != ' ' && end[0] != '\0'){
			end++;
		}
		if(start >= end){
			fprintf(stderr, "Enter filename\n");
			exit(EXIT_FAILURE);
		}
		if(end[0] == ' '){
			strcpy(rDirLoc, filename + (end - start + 1)); 
		}else if(end[0] == '\0'){
			rDirLoc[0] = '\0';
		}
		filename[end - start + 1] = '\0';
		// printf("Write to file [%s].\n", filename);

		int fd = open(filename,O_RDONLY);
	    if(fd != -1){
	    	dup2(0,STDIN_BACKUP);
		    STDIN_CHANGED = true;
	      dup2(fd, 0); //Replace stdin
	      close(fd);
	      // close(1);
	    }else{
	      perror("open");
	    }
	}


	if((rDirLoc = strstr(original, ">>")) != NULL){
		//arg = strtok(arg," ");
		arg = rDirLoc + 2;
		if(rDirLoc == '\0'){
			fprintf(stderr, "Enter filename\n");
			exit(EXIT_FAILURE);
		}
		arg = trim(arg);
		char filename[256];

		char *start = arg;
		printf("%s\n", start);
		strcpy(filename, start);
		char *end = start;
		while(end[0] != ' ' && end[0] != '\0'){
			end++;
		}
		if(start >= end){
			fprintf(stderr, "Enter filename\n");
			exit(EXIT_FAILURE);
		}
		if(end[0] == ' '){
			strcpy(rDirLoc, filename + (end - start + 1)); 
		}else if(end[0] == '\0'){
			rDirLoc[0] = '\0';
		}
		filename[end - start + 1] = '\0';
		// printf("Write to file [%s].\n", filename);

		int fd = open(filename,O_WRONLY|O_APPEND|O_CREAT,S_IRWXU);
	    if(fd != -1){
	    	dup2(1,STDOUT_BACKUP);
		    STDOUT_CHANGED = true;
	      if(dup2(fd, 1) == 0){ //Replace stdout
		      close(fd);
		  }else{
		  	perror("dup2");
		  }
	      // close(1);
	    }else{
	      perror("open");
	    }
	}
	// printf("After redirection: %s\n", original);
}

void lookForPipe(char* arg){
	char *pipeLoc;

	while((pipeLoc = strstr(arg, "|")) != NULL){

		char *nxtCmd = pipeLoc + 1;
		nxtCmd = trim(nxtCmd);
		if(strlen(nxtCmd) <= 0){
			fprintf(stderr, "Second Command needed\n");
		}
		pipeLoc+=1;
	}
}

void handlePipe_Parent(bool pipeRequired, bool pipeHistory){
	STDOUT_BACKUP = 11;
	STDIN_BACKUP = 10;
	if(pipeHistory){
		//Backup stdin once
		if(!STDIN_CHANGED){
			dup2(0,STDIN_BACKUP);		
		}
		
		//Close the opened write end @ 1
		close(0);
		dup2(fd_a[0], 0);
		close(fd_a[0]); //Close pipe's read end [original instance]
		STDIN_CHANGED = true;
	}
	if(pipeRequired){
		if(!STDOUT_CHANGED){
			//Backup STDOUT
			dup2(1, STDOUT_BACKUP);
			// perror("dup2");
		}
		//In parent create a new pipe for next child to write to
		pipe(fd_a);

		close(1);
		dup2(fd_a[1], 1); //Connect write end to stdout
		close(fd_a[1]); //Close the original instance
		STDOUT_CHANGED = true;
	}
	if(pipeHistory && !pipeRequired){
		//Last pipe
		restoreSTDOUT();
	}
}

void restoreSTDIN(){
	if(STDIN_CHANGED){
		close(0);
		dup2(STDIN_BACKUP, 0);
		close(STDIN_BACKUP);
		// perror("dup2");
		STDIN_CHANGED = false;
	}
}

void restoreSTDOUT(){
	if(STDOUT_CHANGED){
		close(1);
		//Copy STDOUT to 1
		dup2(STDOUT_BACKUP, 1);
		close(STDOUT_BACKUP);
		// perror("dup2");
		STDOUT_CHANGED = false;
		printf("Closing fd 1\n");
	}
}