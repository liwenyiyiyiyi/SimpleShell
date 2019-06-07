#include "parse.h"
#define MAX 1024


void parseCommand(char* cmdLine,char *symbol,char* result[]){
  int i = 0;
  /*array of parameter*/
  for(;i  < MAX;i++){
    /*separate the command line by symbol*/
    if(cmdLine == NULL){
      break;
    }

    if (strlen(cmdLine) == 0){
        break;
    }
    /*printf("cmdLine = %s\n",cmdLine);*/
    /*strcpy(result[i],strsep(&cmdLine, symbol));*/
    result[i] = strsep(&cmdLine, symbol);
    if (strlen(result[i]) == 0){
      i--;
    }

    /*reach end*/
  }
  result[i] = NULL;
  /*printf("out loop command[0]  = %s\n",result[0]);*/
}

int isBuiltInCommand(char* cmd[]) {
  /*printf("enter judge built_in\n");*/

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

char **redirectionCommand(char *cmd[])
{
  int flag_In = 0;
  int flag_Out = 0;
  int flag_Outplus = 0;
  int counter = 0;
  int file_In;
  int file_Out;
  int file_Outplus;
  char In[MAX];
  char Out[MAX];
  char Outplus[MAX];
  char *token = cmd[0];
  /*printf("enter redirection\n");*/

  while (token != NULL)
  {
    /*printf("token = %s\n",token);*/
    /*printf("counter = %d\n",counter);*/
    /* "< " detected*/
    if (strncmp(token, ">>", 2) == 0)
    {
      cmd[counter] = NULL;
      flag_Outplus = 1;
      strcpy(Outplus, cmd[counter + 1]);
    }else if (strncmp(token, ">", 1) == 0)
    {
      cmd[counter] = NULL;
      flag_Out = 1;
      strcpy(Out, cmd[counter + 1]);
    }else if (strncmp(token, "<", 1) == 0)
    {
      cmd[counter] = NULL;
      flag_In = 1;
      strcpy(In, cmd[counter + 1]);
    }

    token = cmd[++counter];
  }
  /*printf("flag_In = %d,flag_Out = %d,flag_Outplus = %d\n ",flag_In,flag_Out,flag_Outplus );*/

  if (!flag_In && !flag_Out && !flag_Outplus)
  {
    /*printf("no redirection!\n");*/
    return cmd;
  }

  if (flag_In)
  {
    file_In = open(In, O_RDONLY, 0);
    dup2(file_In,0);
    close(file_In);
  }
  if (flag_Out)
  {
    file_Out = open(Out,O_WRONLY | O_TRUNC | O_CREAT, 0644);
    dup2(file_Out,1);
    close(file_Out);
  }
  if (flag_Outplus)
  {

    file_Outplus = open(Outplus, O_WRONLY | O_CREAT | O_APPEND, 0666);
    dup2(file_Outplus,1);
    close(file_Outplus);
  }
  /*printf("cmd[0] = %s\n",cmd[0]);*/
  return cmd;
}
