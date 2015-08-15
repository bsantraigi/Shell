/** @file mybash.c
 *  @brief A simple shell
 *
 *  This file waits for the user to 
 *	input a command. Then if the command 
 *	is implemented it creates a child process
 *	using fork() and executes the corresponding
 *	executable file in the child process.
 *
 *  @author Bishal Santra
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "utility.h"
#include "myterminal.h"

extern int errno;

/** @brief Reads the input from stdin until "Enter" key is pressed
 *
 *  Reads a single line input from stdin till a '\n' and returns
 *	a pointer to it
 *
 *  @param string The variable to store the input
 *  @param BUFF_SIZE Max size for input string
 *  @return Pointer to the input string
 */
char* readLine(char* string, int BUFF_SIZE);
void ParentBehave(bool runInBG, int* bgProcessCount, pid_t pid, int *status, int flag);





int main(){
	STDIN_CHANGED = false;
	STDOUT_CHANGED = false;
	// ConfigureTerminal();
	initHistory();

	char shellLoc[1024];
	getcwd(shellLoc,1024);

	char pwd[1024];
	sprintf((char*)pwd, "%s/mypwd", shellLoc);

	char ls[1024];
	sprintf((char*)ls, "%s/myls", shellLoc);

	char mkdir[1024];
	sprintf((char*)mkdir, "%s/mymkdir", shellLoc);

	char rm[1024];
	sprintf((char*)rm, "%s/myrm", shellLoc);

	char tail[1024];
	sprintf((char*)tail, "%s/mytail", shellLoc);

	char mv[1024];
	sprintf((char*)mv, "%s/mymv", shellLoc);

	char ps[1024];
	sprintf((char*)ps, "%s/myps", shellLoc);


	// char *command;
	char *args;
	char *savePtrPipe;
	char temp[256];
	bool pipeRequired = false;
	bool pipeHistory = false;
	// command = (char *)malloc(256*sizeof(char));
	pid_t pid;
	int status;
	int bgProcessCount = 0;

	while(1){
		printw("root@PC> ");

		char command_full[256];
		char c_arr[256];
		char *command;
		command = readLine(command, 256); //command may contain piped multiple commands
		command = trim(command);
		if(strcmp(command, "") == 0){
			continue;
		}
		strcpy(command_full, command); //temp will be parsed as the command
		
		command = strtok_r(command, "|", &savePtrPipe);
		char* nxtCommand = strtok_r(NULL, "|", &savePtrPipe);

		do{
			bool runInBG = false;
			if(nxtCommand != NULL){
				//There is another piped command
				//Create a pipe and set a flag				
				pipeHistory = pipeRequired;
				pipeRequired = true;
			}else{
				pipeHistory = pipeRequired;
				pipeRequired = false;
			}
			strcpy(temp, command); //temp will be parsed as the command
	
			//Split the command and its arguments
			command = trim(command);
			if(command[strlen(command) - 1] == '&'){
				//RUN IN BG
				runInBG = true;
				command[strlen(command) - 1] = '\0';
			}
			args = strstr(command, " ");
			if(args != NULL){
				args[0] = '\0';
				args = args + 1;
				args = trim(args);
			}else{
				args = trim(args);
			}
			// printf("No trim: %s + %s--\n", command, args);
			
			handlePipe_Parent(pipeRequired, pipeHistory);
			if(strcmp(command,"mypwd") == 0){
				switch(pid = fork()){
					case 0:
					//Inside child process
					execl(pwd, args, (char*)NULL);
					perror("execv");
					break;
					case -1:
					perror("fork");
					break;
					default:

					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
				}
			}else if(strcmp(command,"myls") == 0){
				checkRedirect(args);
				switch(pid = fork()){
					case 0:
					//Inside child process
					execl(ls, strlen(args)>0?args:(char*)piped_arg, (char*)NULL);
					perror("execl");
					break;
					case -1:
					perror("fork");
					break;
					default:
					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
				}
			}else if(strcmp(command,"myps") == 0){
				checkRedirect(args);
				switch(pid = fork()){
					case 0:
					//Inside child process
					execl(ps, strlen(args)>0?args:(char*)piped_arg, (char*)NULL);
					perror("execl");
					break;
					case -1:
					perror("fork");
					break;
					default:					
					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
				}
			}else if(strcmp(command,"myrm") == 0){
				checkRedirect(args);
				switch(pid = fork()){
					case 0:
					//Inside child process
					execl(rm, strlen(args)>0?args:(char*)piped_arg, (char*)NULL);
					perror("execl");
					break;
					case -1:
					perror("fork");
					break;
					default:					
					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
				}
			}else if(strcmp(command,"mytail") == 0){
				checkRedirect(args);
				switch(pid = fork()){
					case 0:
					//Inside child process
					args = trim(args);
					int i = 0, j = 0;
	
					char* num = strtok(args, " ");
	
					int count = atoi(num);
					if(count == 0){
						execl(tail, args, (char*)NULL);
					}else if(num == args || num!=NULL){
						execl(tail, num, args+strlen(num) + 1, (char*)NULL);
						perror("execl");
					}
					break;
					case -1:
					perror("fork");
					break;
					default:					
					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
				}
			}else if(strcmp(command,"mymkdir") == 0){
				checkRedirect(args);
				switch(pid = fork()){
					case 0:
					//Inside child process
					execl(mkdir, strlen(args)>0?args:(char*)piped_arg, (char*)NULL);
					perror("execl");
					break;
					case -1:
					perror("fork");
					break;
					default:					
					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
				}
			}else if(strcmp(command,"mymv") == 0){
				checkRedirect(args);
				switch(pid = fork()){
					case 0:
					//Inside child process
					execl(mv, strlen(args)>0?args:(char*)piped_arg, (char*)NULL);
					perror("execl");
					break;
					case -1:
					perror("fork");
					break;
					default:					
					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
				}
			}else if(strcmp(command,"mycd") == 0 || strcmp(command,"cd") == 0){
				// printf("Chaning folder to %s\n", args);
				if(chdir(args) == -1){
					perror("chdir");
				/*	switch(errno){
						case ENOTDIR:
							printf("ERROR: That's not a folder.\n");
						break;
						case EBADF:
							printf("ERROR: Bad folder name.\n");
						break;
						default:
							printf("ERROR: Cannot change directory.\n");
					}*/
				}else{
					// printf("Hi\n");
				}
			}else if(strcmp(command,"exit") == 0){
				if(pipeHistory){
					close(0); //Close pipes read end
					dup2(STDIN_BACKUP, 0); //Copy stdin back to 0
					close(STDIN_BACKUP);
				}
				return 0;
			}else if(strcmp(command,"") != 0){
				
				// printf("Shell Command %s\n", command);
				checkRedirect(args);
				switch(pid = fork()){
					case 0:
					// printf("Process waiting for return: (%ld)\n",(long)getpid() );
					// getchar();
					//Inside child process
					args = trim(args);
					// printf("%s\n", strlen(args)>0?args:(char*)piped_arg);
					// execlp(command, command, strlen(args)>0?args:(char*)piped_arg, (char*)NULL);
					execlp(command, command, strlen(args)>0?args:(char*)NULL, (char*)NULL);
					perror("execlp");
					exit(EXIT_FAILURE);				
					break;
					case -1:
					perror("fork");
					break;
					default:
					// printf("Waiting for %ld\n",(long)pid );
					ParentBehave(runInBG, &bgProcessCount, pid, &status, 0);
					/*
					if(!runInBG){
						
						waitpid(pid,&status,WUNTRACED|WCONTINUED);
					}*/				
				}
				// fprintf(stderr, "%s is not a valid command.\n", command);
			}
			command = nxtCommand;
			nxtCommand = strtok_r(NULL, "|", &savePtrPipe);
		}while(command != NULL);

		restoreSTDIN();
		restoreSTDOUT();
	}
	// ResetTerminal();
	return 0;
}

char* readLine(char* string, int BUFF_SIZE){
	//Freeing cmd/string is not required because they simply point to 
	//the <inBuff1.vals> which is statically allocated char[1024]
	char *cmd = pollForInput();	
	return cmd;
}






void ParentBehave(bool runInBG, int* bgProcessCount, pid_t pid, int *status, int flag){
	if(!runInBG){
		waitpid(pid,status,flag);
	}else{
		*bgProcessCount = *bgProcessCount+1;
		printf("[%d] %ld\n",*bgProcessCount, (long)pid);		
	}
}