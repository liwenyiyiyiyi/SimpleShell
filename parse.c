#include "parse.h"
#define MAX 1024
char* readCommandLine(char *cmdLine){
  char *cmdBuf;
  cmdBuf = readline(" ");
  if (strlen(cmdBuf) != 0)
  {
    strcpy(cmdLine,cmdBuf);
    return 0;
  }else{
    return 1;
  }
}

char** parseCommand(char* cmdLine){
  int i = 0;
  //array of parameter
  char** command;
  for(;i  < 100;i++){
    /*separate the command line by space*/
    command[i] = strsep(&cmdLine, " ");
    /*reach end*/
    if (command[i] == NULL){
      break;
    }

  }
  return command;
}

bool isBuiltInCommand(char** cmd) {
  if (strcmp(cmd[0], "cd") == 0) {
    return 1;
  } else if (strcmp(cmd[0], "exit") == 0) {
    return 1;
  } else if (strcmp(cmd[0], "jobs") == 0) {
    return 1;
  } else if (strcmp(cmd[0], "kill") == 0) {
    return 1;
  } else {
    return 0;
  }
}

char **redirectionCommand(char **cmd)
{
    int flag_In = 0;
    int flag_Out = 0;
    int flag_Outplus = 0;
    int counter = 0;
    int length;
    int file_In;
    int file_Out;
    int file_Outplus;
    char In[MAX];
    char Out[MAX];
    char Outplus[MAX];
    char *token = cmd[0];
    while (token)
    {
        length = strlen(token);
        if (!strncmp(token, "<", length))
        {
            cmd[counter] = NULL;
            flag_In = 1;
            strcpy(In, cmd[counter + 1]);
        }
        if (!strncmp(token, ">", length))
        {
            cmd[counter] = NULL;
            flag_Out = 1;
            strcpy(Out, cmd[counter + 1]);
        }
        if (!strncmp(token, ">>", length))
        {
            cmd[counter] = NULL;
            flag_Outplus = 1;
            strcpy(Outplus, cmd[counter + 1]);
        }
        counter++;
        token = cmd[counter];
    }
    if (!flag_In && !flag_Out && !flag_Outplus)
    {
        return cmd;
    }
    if (flag_In)
    {

        file_In = open(In, O_RDONLY, 0);
        dup(file_In);
        close(file_In);
    }
    if (flag_Out)
    {

        file_Out = open(Out, O_TRUNC | O_CREAT, 0644);
        dup(file_Out);
        close(file_Out);
    }
    if (flag_Outplus)
    {

        file_Outplus = open(Outplus, O_WRONLY | O_CREAT | O_APPEND, 0666);
        dup(file_Outplus);
        close(file_Outplus);
    }
    return cmd;
}
