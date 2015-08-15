/** @file myterminal.c
 *  @brief Implementation of TERMINAL INPUT behaviour and BASH HISTORY
 *
 *  Using termios data structure and library functions 
 *  we override the behaviour of the default bash/shell 
 *  to get a non-blocking input behaviour and maintain 
 *  a buffer ourselves. When enter is pressed we run the
 *  command store in the buffer.
 *
 *  History is also store in a dynamically allocated 2D 
 *  character array. Whenever the history table fills up
 *  this array is reallocated.
 *  
 *  @author Bishal Santra
 */
#include "myterminal.h"

CharStack inBuff1, inBuff2;
struct termios termios_p;
struct termios termios_save;

void ConfigureTerminal(){
    tcgetattr(0, &termios_p);
    termios_save = termios_p;
    termios_p.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(0,TCSANOW, &termios_p);
}
void ResetTerminal(){
    tcsetattr(0,TCSANOW, &termios_save);
}

void printw(char* seq){
    write(STDOUT_FILENO, seq, strlen(seq));
}

char* pollForInput(){    
    resetHistory();
    ConfigureTerminal();

    inBuff2.size = 0;
    inBuff1.size = 0;

    char buff;
    while(read(0, &buff, 1) >= 0){
        if(buff > 0){
            if(buff == '\n'){
                write(STDOUT_FILENO,&buff,1);                
                break;
            }
            if(buff == ESCAPE){
                read(0, &buff, 1);
                read(0, &buff, 1);
                if(buff == 'A'){
                    // UP Pressed
                    char* lastCmd = up_hEntry();
                    if(lastCmd != NULL){
                        while(inBuff1.size > 0){
                            write(STDOUT_FILENO, "\10\33[1P", 5);
                            pop(&inBuff1);
                        }
                        while(inBuff2.size > 0){
                            write(STDOUT_FILENO, "\33[1P", 4);
                            pop(&inBuff2);
                        }
                        setBuff(&inBuff1, lastCmd);
                        setBuff(&inBuff2, "");
                        printw(inBuff1.vals);
                    }
                }else if(buff == 'B'){
                    // DOWN Pressed
                    //Clear line
                    char* lastCmd = down_hEntry();
                    if(lastCmd != NULL){
                        while(inBuff1.size > 0){
                            write(STDOUT_FILENO, "\10\33[1P", 5);
                            pop(&inBuff1);
                        }
                        while(inBuff2.size > 0){
                            write(STDOUT_FILENO, "\33[1P", 4);
                            pop(&inBuff2);
                        }
                        //Retrieve and Print the last command
                        setBuff(&inBuff1, lastCmd);
                        setBuff(&inBuff2, "");
                        printw(inBuff1.vals);
                    }
                }else if(buff == 'C'){
                    if(inBuff2.size > 0){
                        write(STDOUT_FILENO, "\33[C", 3); //Right
                        push(&inBuff1,pop(&inBuff2));
                    }
                }else if(buff == 'D'){ //Left arrow pressed
                    if(inBuff1.size > 0){
                        write(STDOUT_FILENO, "\10", 2);
                        push(&inBuff2,pop(&inBuff1));
                    }
                }
            }else if(buff == BACKSPACE){
                if(inBuff1.size > 0){
                    write(STDOUT_FILENO, "\10\33[1P", 5);
                    pop(&inBuff1);
                }
                
            }else if(buff == DELETE){
                if(inBuff2.size > 0){
                    write(STDOUT_FILENO, "\33[1P", 4);
                    pop(&inBuff2);
                }
            }else{
                write(STDOUT_FILENO,&buff,1);
                push(&inBuff1,buff);
                printBuff(&inBuff2);
            }
            // write(STDOUT_FILENO,&buff,1);
            
        }
    }
    ResetTerminal();
    int i;
    // printf("\nSizes: [%d] [%d]\n", inBuff1.size, inBuff2.size);
    while(inBuff2.size > 0){
        push(&inBuff1,pop(&inBuff2));
    }


    int l = inBuff1.size;
    if(l > 0){
        push(&inBuff1, '\0');
        //If last command and current commands are not same add
        //a new entry to the history stack
        if(hStack1.size==0?1:(strcmp(inBuff1.vals, hStack1.entries[hStack1.size - 1]) != 0)){
            char *lastCmd = (char*)malloc((l + 1)*sizeof(char));
            strcpy(lastCmd, inBuff1.vals);
            push_hEntry(lastCmd, 1);
        }
        return inBuff1.vals;
    }else{
        return "\n";
    }
    /*for (i = 0; i < inBuff1.size; ++i)
    {
        
    }*/
    // printf("\nSizes: [%d] [%d]\n", inBuff1.size, inBuff2.size);
    // printf("\nFinal Buffer Content: %s\n", inBuff1.vals);
    
    
}

void push(CharStack* s, char val){
    if(s->size < 1024){
        s->vals[s->size] = val;
        s->size++;
    }else{
        fprintf(stderr, "Buffer Overflow\n");
        exit(EXIT_FAILURE);
    }
}

char pop(CharStack* s){
    if(s->size >= 1){
        s->size--;
        return s->vals[s->size];
    }
    return -1;
}

char peek(CharStack* s){
    if(s->size >= 0){
        return s->vals[s->size];
    }
    return -1;
}

void setBuff(CharStack* s, char* val){
    strcpy(s->vals, val);
    s->size = strlen(s->vals);
}

//Does not erase the buffer
void printBuff(CharStack* buff){
    int i;
    /*if(buff->size > 0){
        write(STDOUT_FILENO, "\33[C", 3); //Right
    }*/
    for (i = buff->size-1; i >= 0; --i)
    {
        write(STDOUT_FILENO, buff->vals+i, 1);
    }
    for (i = buff->size-1; i >= 0; --i)
    {
        write(STDOUT_FILENO, "\10", 1); //Go left
    }
}

void initHistory(){
    hStack1.maxSize = 2;
    hStack1.size = 0;
    hStack1.entries = (char**)malloc(hStack1.maxSize*sizeof(char*));
    hStack1.currentPos = 0;

    hStack2.maxSize = 2;
    hStack2.size = 0;
    hStack2.currentPos = 0;
    hStack2.entries = (char**)malloc(hStack2.maxSize*sizeof(char*));
}

void push_hEntry(char* lastCmd, int stackID){
    HistoryStack *hStack;
    hStack = stackID==1?&hStack1:stackID==2?&hStack2:NULL;
    
    if(hStack->size == hStack->maxSize){
        hStack->maxSize *= 2;
        hStack->entries = (char**)realloc(hStack->entries, hStack->maxSize*sizeof(char*));
    }
    hStack->entries[hStack->size] = lastCmd;
    hStack->size++;
    hStack->currentPos = hStack->size;
}

char* pop_hEntry(int stackID){
    HistoryStack *hStack;
    hStack = stackID==1?&hStack1:stackID==2?&hStack2:NULL;
    hStack->size--;
    char *temp = hStack->entries[hStack->size];
    hStack->entries[hStack->size] = NULL;
    return temp;
}

char* up_hEntry(){
    if(hStack1.size == 0){
        return NULL;
    }
    if(hStack1.currentPos == 0){
        return NULL;
    }
    char* lastCmd = hStack1.entries[--hStack1.currentPos];
    // push_hEntry(lastCmd, 2);
    return lastCmd;
}

char* down_hEntry(){
    if(hStack1.size == 0){
        return NULL;
    }
    if(hStack1.currentPos == hStack1.size){
        return NULL;
    }
    char* nextCmd = hStack1.entries[hStack1.currentPos++];
    return nextCmd;
}

void resetHistory(){
    hStack1.currentPos = hStack1.size;
}