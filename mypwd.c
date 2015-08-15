/** @file mypwd.c
 *  @brief Implementation of the standard pwd command
 *
 *	Prints the current working directory
 *
 *  @author Bishal Santra
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/** @brief Main of myps program
 *	
 *	First it gets the pid of the process using getpid() function.
 *	Reads the /proc/[pid]/cwd file to get the current
 *	working directory of this process
 *
 *  @return int(0)
 */
int main(){
	char buffer[50];
	// getcwd(buffer,50);
	pid_t pid = getpid();
	char symLink[20];
	sprintf(symLink, "/proc/%ld/cwd",(long)pid);	
	ssize_t size;
	size = readlink(symLink, buffer,50);
	buffer[(int)size] = '\0';
	printf("%s\n", buffer);
	exit(EXIT_SUCCESS);
}