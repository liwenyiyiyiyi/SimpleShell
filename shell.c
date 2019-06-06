#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include "parse.c"
#define MAX 1024
#define MAXarg 64
#define MAXpipe 32

void printPrompt(){

  char cwd[MAX];
  char host[MAX];
  char *user;
  char *env = "USER";
  char dest[MAX];
  int i;
  user = getenv(env);
  gethostname(host,sizeof(host));
  getcwd(cwd, sizeof(cwd));
  i = strlen(user);
  strncpy(dest, cwd+6+i);
  printf("\n%s@%s:~%s$",user ,host, dest);
}


void executeBuiltInCommand(char** cmd){
  if (strcmp(cmd[0],"cd") == 0 ){
    chdir(cmd[1]);
  }else if (strcmp(cmd[0],"exit") == 0){
    return 0;
  }else if (strcmp(cmd[0],"jobs") == 0){

  }else if (strcmp(cmd[0], "kill") == 0){

  }
}



void executeCommand(char** cmd){
  execvp(cmd[0],cmd);
}


bool isBackgroundJob(char* cmd);


int main(){
  int flag = 0;
  char cmdLine[MAX];
  char *cmd[MAXarg];
  while(1){

    int childPid;

    printPrompt();

    flag = readCommandLine(cmdLine); /*or GNU readline("");*/
    if (flag){continue;}
    cmd = parseCommand(cmdLine);

    if ( isBuiltInCommand(cmd)){
      executeBuiltInCommand(cmd);
    } else {
      childPid = fork();
      if (childPid == 0){
        executeCommand(cmd); /*calls execvp*/

      } else {
        if (isBackgroundJob(cmd)){
          /*record in list of background jobs*/

        } else {
          waitpid (childPid);

        }
      }
    }


  }
}
