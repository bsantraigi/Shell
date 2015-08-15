/** @file myrm.c
 *  @brief Implementation of the standard rm command
 *
 *  Removes a file or directory
 *	myrm [OPTION] [FPATH]
 *	-r: Remove a non-empty directory and everything inside it
 *	-d: Remove a empty directory
 *
 *  @author Bishal Santra
 */
#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include "utility.h"

char parse(char* arg);
void deleteDir(char* dirName, int isRecursive);


/** @brief DeleteCallback - Deletes a file/directory.
 *
 *  A pointer to this function is passed to nftw.
 *	While the file tree walk nftw calls this function for
 *	each file and directory within the directory tree.
 *
 *  @return int FtW_CONTINUE(a macro) instructs the nftw function
 *	to continue normally
 */
static int
deleteCallback(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{

    /*printf("%-3s %2d %7jd   %-40s %d %s\n",
        (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
        (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
        (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
        (tflag == FTW_SLN) ? "sln" : "???",
        ftwbuf->level, (intmax_t) sb->st_size,
        fpath, ftwbuf->base, fpath + ftwbuf->base);*/
    if(remove(fpath) == -1){
    	perror(fpath);
    }else{
    	printf("'%s' has been removed.\n", fpath);
    }
    if(tflag == FTW_D){
      printf("Bad\n");
      exit(0);
      return FTW_SKIP_SUBTREE;
    }else{
      return FTW_CONTINUE;
    }
    return 0;           /* To tell nftw() to continue */
}

int main(int argc, char *argv[]){
	argv[0] = trim(argv[0]);
	char c = parse(argv[0]);
	if(c == 'd'){
		char* dirName = argv[0] + 3;
		dirName = trim(dirName);
		printf("Command: -d %s\n", dirName);
		strtok(dirName," ");
		while(dirName != NULL){
			/*printf("Current Dir: %s\n", dirName);
			DIR* dir;
			struct dirent *rddir;
			dir = opendir(dirName);
			if(dir == NULL){
				perror("opendir");
			}else{				
				int ret = remove(dirName);
				if(ret == -1){
					perror("remove");
				}				
			}*/
			if(strlen(dirName) > 0){
				deleteDir(dirName, 0);
			}

			dirName = strtok(NULL," ");
		}
	}else if(c == 'r'){
		char* dirName = argv[0] + 3;
		dirName = trim(dirName);
		printf("Command: -r %s\n", dirName);
		strtok(dirName," ");
		while(dirName != NULL){
			/*printf("Current Dir: %s\n", dirName);
			DIR* dir;
			struct dirent *rddir;
			dir = opendir(dirName);
			if(dir == NULL){
				perror("opendir");
			}else{				
				int ret = remove(dirName);
				if(ret == -1){
					perror("remove");
				}				
			}*/
			if(strlen(dirName) > 0){
				deleteDir(dirName, 1);
			}

			dirName = strtok(NULL," ");
		}
	}else if(c == ' '){
		char* fileName = argv[0];
		fileName = trim(fileName);
		printf("Command: %s\n", fileName);
		strtok(fileName," ");
		while(fileName != NULL){
			printf("Current File: %s\n", fileName);
			struct stat s;
			if(stat(fileName, &s) == 0){
				if(S_ISDIR(s.st_mode)){
					// printf("'%s' is a directory.\n", fileName);
				}else{
					// printf("'%s' is a file.\n", fileName);
					if(remove(fileName) == -1){
						perror("remove");
					}
				}
			}else{
				perror("stat");
			}
			/*DIR* dir;
			struct dirent *rddir;
			dir = opendir(fileName);
			if(dir == NULL){
				perror("opendir");
			}else{				
				int ret = remove(fileName);
				if(ret == -1){
					perror("remove");
				}				
			}*/

			fileName = strtok(NULL," ");
			//fileName = trim(fileName);
		}
	}
	return 0;
}

/** @brief Parse
 *
 *  Check whether any options has been passed in the argument
 *
 *  @param arg -commandline argument
 *  @return char - 'd' or 'r'
 */

char parse(char* arg){
	if(arg[0] == '-'){
		if(arg[1] == 'd'){
			arg += 2;
			return 'd';
		}else if(arg[1] == 'r'){
			arg += 2;
			return 'r';
		}
	}else{
		return ' ';
	}
}

void deleteFile(char* fileName){

}

/** @brief deleteDir - Deletes a empty or non-empty directory
 *
 *  Deletes the directory mentioned in dirName. Removes all
 *	files and directories recursively with the help of nftw if
 *	the directory is not empty.
 *
 *  @param dirName - absolute/relative path of the directory
 *  @param recursive - to delete a non-empty directory set this to 1
 *	@return void
 */

void deleteDir(char* dirName, int isRecursive){
	if(isRecursive == 1){
		int flags = 0;

	    flags |= (FTW_DEPTH|FTW_PHYS|FTW_ACTIONRETVAL);
	    
	    if (nftw(dirName, deleteCallback, 20, flags)
	            == -1) {
	        perror("nftw");
	        // exit(EXIT_FAILURE);
	    }
	    // exit(EXIT_SUCCESS);
	}else{
		printf("Current Dir: -d %s\n", dirName);
		DIR* dir;
		struct dirent *rddir;
		dir = opendir(dirName);
		if(dir == NULL){
			perror("opendir");
		}else{
			int n = 0;
			while((rddir = readdir(dir))){
				n++;
				if(n > 2){
					break;
				}
			};
			if(n <= 2){
				int ret = remove(dirName);
				if(ret == -1){
					perror("remove");
				}
			}else{
				fprintf(stderr, "Directory '%s' is not empty.\n", dirName);
			}
			
		}
	}


	
}

