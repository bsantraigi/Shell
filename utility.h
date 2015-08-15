#pragma once
#include <stdbool.h>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

/** @brief Trim String
 *
 *  Trim leading and trailing spaces from a string
 *  
 *  @return Void
 */
char* trim(char* str);

/** @brief Check Redirection operators
 *
 *  Look for redirection operators and set file descriptor table
 *	accordingly
 *  
 *  @return Void
 */
void checkRedirect(char* arg);

/** @brief Handle Pipe
 *
 *  Create pipes and use them properly to pass output of the leading
 *	program to the following program
 *  
 *  @return Void
 */
void handlePipe_Parent(bool pipeRequired,bool pipeHistory);

/** @brief Restore STDIN
 *
 *  Put STDIN back to index 0 in FD Table
 *  
 *  @return Void
 */
 
void restoreSTDIN();
/** @brief Restore STDOUT
 *
 *  Put STDOUT back to index 1 in FD Table
 *  
 *  @return Void
 */
void restoreSTDOUT();

//Global Variables
int fd_a[2];
int fd_b[2];
bool stdout_changed;
char *piped_arg;

int STDIN_BACKUP;
int STDOUT_BACKUP;
bool STDIN_CHANGED;
bool STDOUT_CHANGED;
