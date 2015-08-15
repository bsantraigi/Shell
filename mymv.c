/** @file mymv.c
 *  @brief Implementation of the standard mv command
 *
 *  Moves a file or the contents in a directory
 *	mymv [SOURCE] [DESTINATION]
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

char destDir[1024];
int destSize;

/** @brief movFile - moves a file
 *
 *  Moves a file from source to destination
 *
 *	@param srcFName - the absolute/relative path of the file while in source location
 *	@param destFName - the absolute/relative path of the file while in destination location
 *	@param echoOff - if set to 1, perror(..) messages will be printed
 *  @return void
 */
void movFile(const char* srcFName,const char* destFName, int echoOff){
	if(rename(srcFName, destFName) == -1){
		if(!echoOff){
			perror("rename");
		}
	}
}

/** @brief MovFile_Callback - moves a file
 *
 *	This function is a callback for the nftw function. For each file it runs movFile(..) function.
 *  Moves a file from source to destination. Call the movFile function internally.
 *
 *  @return int FtW_CONTINUE(a macro) instructs the nftw function
 *	to continue normally
 */

static int
MovFile_Callback(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
    // printf("Moving %s\n", fpath);
    char fullDest[512];
	sprintf(fullDest, "%s/%s", destDir, fpath + destSize + 1);
	// printf("%s\n", fullDest);
	movFile(fpath, fullDest, 1);
	if(S_ISDIR(sb->st_mode)){
		remove(fpath);
	}
	return FTW_CONTINUE;
	// return FTW_CONTINUE;
/*
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
    }*/
}


int main(int argc, char *argv[])
{
	if(argc >= 1){
		argv[0] = trim(argv[0]);
		char *source = strtok(argv[0]," ");
		char *dest = strtok(NULL," ");
		dest = trim(dest);
		source = trim(source);
		if(strlen(source) == 0 || strlen(dest) == 0){
			exit(0);
		}

		struct stat sb;
		if(stat(source, &sb) == 0){
			if(S_ISDIR(sb.st_mode)){
				//nftw call
				if(stat(dest,&sb)==0){
					if(S_ISDIR(sb.st_mode)){
					}else{
						fprintf(stderr, "Destination has to be a directory also.\n");
						exit(EXIT_FAILURE);
					}
					
				}
				strcpy(destDir, dest);
				destSize = strlen(destDir);
				int flags = 0;

			    flags |= (FTW_DEPTH|FTW_PHYS|FTW_ACTIONRETVAL);
			    
			    if (nftw(source, MovFile_Callback, 20, flags) == -1) {
			        perror("nftw");
			        // exit(EXIT_FAILURE);
			    }

			}else{
				//source is file
				if(stat(dest,&sb)==0){
					if(S_ISDIR(sb.st_mode)){
						char fullDest[512];
						sprintf(fullDest, "%s/%s", dest, source);
						movFile(source, fullDest, 0);
					}else{
						movFile(source, dest, 0);
					}
				}else{
					movFile(source, dest, 0);
				}
			}
		}
		
	}
	return 0;
}