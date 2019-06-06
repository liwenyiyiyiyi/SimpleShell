#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>


int readCommandLine(char *cmdLine);
char** parseCommand(char* cmdLine,char *symbol);
int isBuiltInCommand(char** cmd);
