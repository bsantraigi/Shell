/** @file mytail.c
 *  @brief Implementation of the standard tail command
 *
 *  Print the last 10 lines of the input FILE to standard output. 
 *
 *  @author Bishal Santra
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"



/** @brief Mian of mytail program
 *
 *  Given a filename it opens the 
 file in readmode and seeks to the end of the file
 and then starts reading backward till linecount newlines
 have been encountered.
 *
 *  @param arg[0] line count
 *  @param arg[1] filename
 *  @return int(0)
 */
int main(int argc, char *argv[])
{

	int lines;

	
	char* fileName;
	if(argc > 1){
		lines = atoi(argv[0]);
		lines = lines>0?lines:-lines;
		fileName = argv[1];
	}else{
		lines = 10;
		fileName = argv[0];
	}
	fileName = trim(fileName);
	FILE* file = fopen(fileName, "r");
    int count = 0;
    long int pos;
    char s[100];

    /* always check return of fopen */
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    pos = ftell(file);
    /* Don't write each char on output.txt, just search for '\n' */
    while (pos) {
        fseek(file, --pos, SEEK_SET); /* seek from begin */
        if (fgetc(file) == '\n') {
            if (count++ == lines) break;
        }
    }
    /* Write line by line, is faster than fputc for each char */
    while (fgets(s, sizeof(s), file) != NULL) {
        printf("%s", s);
    }
    fclose(file);
    return 0;
}