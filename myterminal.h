#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define ESCAPE '\33'
#define BACKSPACE '\177'
#define DELETE '~'

typedef struct _stack{
    char vals[1024];
    int size;
} CharStack;
typedef struct _history{
	int size;
	int maxSize;
	int currentPos;
	char **entries;
} HistoryStack;

HistoryStack hStack1;
HistoryStack hStack2;

/** @brief Configure terminal
 *
 *  Configure terminal to store/manipulate the input into custom buffer
 *  
 *  @return Void
 */
void ConfigureTerminal();

/** @brief Reset terminal behaviour
 *
 *  Restore the terminal's default behaviour
 *  
 *  @return Void
 */
void ResetTerminal();

/** @brief Poll for input
 *
 *  Continuosly check for key presses and stores/manipulates characters 
 *	in the buffer stacks and on enter/return key press returns a pointer the
 *	input string.
 *  
 *  @return Void
 */
char* pollForInput();

/** @brief PUSH/POP/PEEK
 *
 *  Stack helper functions
 *  
 *  @return PUSH(void), POP(Char*), PEEK(Char*)
 */
void push(CharStack* s, char val);
char pop(CharStack* s);
char peek(CharStack* s);
/** @brief Printbuff
 *
 *  Prints the buffer in the Stack and returns the cursor 
 *	to it's starting position(when called)
 *  
 *  @return Void
 */
void printBuff(CharStack* buff);
/** @brief Print
 *
 *  Similar to printf function, used when default terminal
 *	behaviours are not active
 *  
 *  @return Void
 */
void printw(char* seq);
/** @brief SetBuff
 *
 *  Set the buffer content to the second argument
 *  
 *  @return Void
 */
void setBuff(CharStack* s, char* val);

//HISTORY FUNCTIONS

/** @brief Initialize History
 *
 *  Initialize the history stack and other counter 
 *	variables
 *  
 *  @return Void
 */
void initHistory();

/** @brief Push H Entry
 *
 * 	Add last command to entry table
 *  
 *  @return Void
 */
void push_hEntry(char* lastCmd, int stackID);
char* pop_hEntry(int stackID);

/** @brief Get entry on UP arrow press
 *
 * 	Get the previous commands serially, while keeping track of
 *	up and down key presses
 *  
 *  @return Void
 */
char* up_hEntry();

/** @brief Get entry on DOWN arrow press
 *
 * 	Get the next commands serially, while keeping track of
 *	up and down key presses
 *  
 *  @return Void
 */
char* down_hEntry();

/** @brief Reset History
 *
 * 	Reset the history counter variables
 *  
 *  @return Void
 */
void resetHistory();