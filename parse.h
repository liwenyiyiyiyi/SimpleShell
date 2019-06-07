#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include<unistd.h>

#include<sys/wait.h>

void parseCommand(char* cmdLine,char *symbol,char* result[]);
int isBuiltInCommand(char** cmd);
char **redirectionCommand(char *cmd[]);
