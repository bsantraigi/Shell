/** @file myps.c
 *  @brief Implementation of the standard ps command
 *
 Lists all the processes running in the system
 *	myps [OPTION]
 *	Default(Lists the processes started by the current user)
 *	-a: Lists all the processes regardless of the terminal user
 *
 *  @author Bishal Santra
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <grp.h>
#include "utility.h"

#define MAX_FILENAME_LEN 256
extern int errno;

void quickSortMain(char **items, int count);
void quickSort(char **items, int left, int right);
char **AllocateMem(char **files);

/** @brief Main of myps program
 *
 *  Reads through the directory /proc and from all the process
 *	directories it reads and parses the following files
 *	--/proc/[pid]/stat -> for pid, tty/pts dev, running time, etc.
 *	--/proc/[pid]/status -> for UID
 *	--/proc/[pid]/cmdline -> for exe path
 *
 *  @param arg[0] <optional> -a
 *  @return int(0)
 */
int main(int argc, char* argv[]){
	int uid = (int)getuid();
	// printf("Current UID: %d\n", uid);
	int checkUID = 1;
	if(argc >= 1){
		argv[0] = trim(argv[0]);
		if(strcmp(argv[0], "-a") == 0){
			checkUID = 0;
			printf("%s\n", "Hi");
		}
	}
	double hertz = sysconf(_SC_CLK_TCK);
	char *cwd = "/proc";
	// getcwd(cwd,50);
	pid_t pid = getpid();
	// printf("Listing files in: %s\n", cwd);

	DIR* dir;
	struct dirent *rddir;
	dir = opendir(cwd);
	if(dir == NULL){
		switch(errno){
			case ENOTDIR:
				printf("Not a directory.\n");
			break;
			case ENOENT:
				printf("Directory doesn't exist.\n");
			break;
		}
	}else{
		char **files = NULL;
		int count = 0;
		struct stat sb;

		printf("%6s %3s   \t%4s %5s\t%s\n","PID","TTY","STAT","TIME","COMMAND");
		while((rddir = readdir(dir))){
			if(atoi(rddir->d_name) <= 0){
				continue;
			}

			//UID Check
			if(checkUID != 0){
				char statusFile[20];
				sprintf((char*)statusFile, "/proc/%s/status", rddir->d_name);
				FILE* status_f = fopen(statusFile,"r");
				if(status_f == NULL){
					continue;
				}
				int line = 0;
				char *reader;
				reader = (char *)malloc(256*sizeof(char));
				while(line < 8){
					fgets((char*)reader, 256, status_f);

					line++;
				}
				
				reader = strtok(reader,"\t");
				
				reader = strtok(NULL, "\t");
				
				int cuid = atoi(reader);
				fclose(status_f);


				if(uid != cuid){
					continue;
				}
			}
			if((count%10) == 0){
				files = AllocateMem(files);
			}

			char procDir[20];
			char cmdLine[512];
			sprintf((char*)procDir, "/proc/%s/stat", rddir->d_name);
			sprintf((char*)cmdLine, "/proc/%s/cmdline", rddir->d_name);
			FILE* cmd = fopen(cmdLine, "r");
			fgets((char*)cmdLine, 512, cmd);
			fclose(cmd);
			// printf("%d check\n", checkUID);
			FILE* pStat = fopen(procDir,"r");
			// perror("fopen");
			if(pStat != NULL){
				sprintf(files[count], "%s", rddir->d_name);
				// printf("%s\n", procDir);

				int itemIndex = 0;
				char buffer[256];
				char c;
				int i = 0;
				int pid;
				int tty_dev;
				int totalTime = 0;
				char state;
				char exeName[256];

				while((c = fgetc(pStat)) != EOF){
					// printf("%c", c);
					if(c != ' '){
						buffer[i++] = c;
						continue;
					}else{
						buffer[i++] = '\0';
						if(itemIndex == 0){
							//PID
							printf("%6s ", buffer);
						}else if(itemIndex == 1){
							strcpy((char*)exeName, (char*)buffer);
						}else if(itemIndex==2){
							state = buffer[0];
						}else if(itemIndex == 6){
							// printf("(tty_nr)%s ", buffer);
							tty_dev = atoi(buffer);
							int majorD = major(tty_dev);
							int minorD = minor(tty_dev);
							if(majorD == 4){
								printf("%3s/%d", "tty", minorD);
							}else if(majorD == 136){
								printf("%3s/%d", "pts", minorD);
							}else{
								printf("?     ");

							}
							
						}else if(itemIndex == 13){
							totalTime += atoll(buffer);
						}else if(itemIndex == 14){
							totalTime += atoll(buffer);
							
						}else if(itemIndex == 16){
							totalTime += atoll(buffer);
						}else if(itemIndex == 17){
							totalTime += atoll(buffer);
						}

						itemIndex++;

						i = 0;
					}				
				}
				printf("\t%4c", state);
				totalTime = (int)((double)totalTime/hertz);
				printf(" %2d:%02d", (int)totalTime/60, totalTime%60);
				printf("\t%s[%s]\n", cmdLine, exeName);
				/*char all[1024];
				fgets((char*)all, 1024, pStat);*/
				count++;
			}else{
				perror("fopen");
			}
			fclose(pStat);
		}
		int i;
		quickSortMain(files, count);
		for (i = 0; i < count; ++i)
		{
			//printf("%s\n", files[i]);
		}
	}
	closedir(dir);
}

char **AllocateMem(char **files){
	static int size = 10;
	int increment = 10;
	int i;
	files = realloc(files, size*sizeof(char*));
	for (i = size - increment; i < size; ++i)
	{
		files[i] = (char*)malloc(MAX_FILENAME_LEN*sizeof(char));
	}
	size += increment;
	return files;
}

void quickSortMain(char **items, int count)
{
  quickSort(items, 0, count-1);
}

void quickSort(char **items, int left, int right)
{
  int i, j;
  char *x;
  char temp[MAX_FILENAME_LEN];

  i = left;
  j = right;
  x = items[(left+right)/2];

  do {
    while((strcmp(items[i],x) < 0) && (i < right)) {
       i++;
    }
    while((strcmp(items[j],x) > 0) && (j > left)) {
        j--;
    }
    if(i <= j) {
      strcpy(temp, items[i]);
      strcpy(items[i], items[j]);
      strcpy(items[j], temp);
      i++;
      j--;
   }
  } while(i <= j);

  if(left < j) {
     quickSort(items, left, j);
  }
  if(i < right) {
     quickSort(items, i, right);
  }
}


