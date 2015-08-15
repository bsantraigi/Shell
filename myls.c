/** @file myls.c
 *  @brief Implementation of the standard ls (-l) command
 *
 *	Prints the contents of a directory
 *
 *  @author Bishal Santra
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define MAX_FILENAME_LEN 256
extern int errno;

/** @brief quickSortMain - Sorts the file list using quick sort algorithm
 *
 *  @return void
 */
void quickSortMain(char **items, int count);
void quickSort(char **items, int left, int right);

/** @brief AllocateMem - Allocates memory to the file list
 *
 *	Allocates/Re-allocates memory to the file list dynamically when required
 *
 *	@param files - the existing file array(string array)
 *  @return char - pointer to the new files array
 */
char **AllocateMem(char **files);

/** @brief permissinFormat - prints (User,Group,Other) permission for a file
 *
 *	@param p - Calculated from stat.st_mode, the permission bits
 *  @return void
 */
void permissionFormat(int p);

int main(){
	char cwd[50];
	// getcwd(cwd,50);
	pid_t pid = getpid();
	char symLink[20];
	sprintf(symLink, "/proc/%ld/cwd",(long)pid);	
	ssize_t size;
	size = readlink(symLink, cwd,50);
	cwd[(int)size] = '\0';
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
		while((rddir = readdir(dir))){
			if((count%10) == 0){
				files = AllocateMem(files);
			}
			sprintf(files[count], "%s", rddir->d_name);
			
			
			count++;
		}
		int i;
		quickSortMain(files, count);
		for (i = 0; i < count; ++i)
		{
			struct stat sb;
			char temp[1024];
			sprintf(temp, "%s/%s", cwd, files[i]);
			if(stat(temp, &sb) == 0){
				char type;
				if(S_ISREG(sb.st_mode)){
					type = '-';
				}else if(S_ISDIR(sb.st_mode)){
					type = 'd';		           
		       	}
		       	else if(S_ISCHR(sb.st_mode)){
		       		type = 'c';
		       	}
				else if(S_ISLNK(sb.st_mode)){
				type = 'l';
				}
				else if(S_ISFIFO(sb.st_mode)){
				type = 'p';
				}
				else if(S_ISSOCK(sb.st_mode)){
				type = 's';
				}
				else if(S_ISBLK(sb.st_mode)){
				type = 'b';
				}
				else{
					type = 'D';
				}
				printf("%c", type);
				permissionFormat(((int)sb.st_mode) >> 6);
				permissionFormat(((int)sb.st_mode) >> 3);
				permissionFormat((int)sb.st_mode);
				printf(" %2d ", (int)sb.st_nlink);
				printf("%10s", getgrgid(sb.st_gid)->gr_name);
				printf("%10s", getpwuid(sb.st_uid)->pw_name);
				printf("%8d", (int)sb.st_size);	

				struct tm* temp = localtime(&sb.st_mtime);
				char _time[25];
				strftime(_time, 25, "%b %d, %H:%M", temp);
				printf("%20s", _time);
				printf("	%s\n", files[i]);

			}else{
				perror("stat");
			}
			
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

void permissionFormat(int p){
	printf("%c",(p&4)==0?'-':'r');
	printf("%c",(p&2)==0?'-':'w');
	printf("%c",(p&1)==0?'-':'x');
}
