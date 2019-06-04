#include "parse.h"
char* readCommandLine(char *cmdLine){
  char *cmdBuf;
  cmdBuf = readline(" ");
  if (strlen(cmdBuf) != 0)
  {
    strcpy(cmdLine,cmdBuf);
    return 0;
  }
  else
  {
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


bool isBuiltInCommand(char** cmd){
  if (strcmp(cmd[0],"cd") == 0 ){
    return 1;
  }else if (strcmp(cmd[0],"exit") == 0){
    return 1;
  }else if (strcmp(cmd[0],"jobs") == 0){
    return 1;
  }else if (strcmp(cmd[0], "kill") == 0){
    return 1;
  }else{
    return 0;
  }

}