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

void printPrompt()
{
  char cwd[1024];
  char host[1024];
  char *user;
  char *env = "USER";
  user = getenv(env);
  gethostname(host,sizeof(host));
  getcwd(cwd, sizeof(cwd));
  char dest[1024];
  int i = strlen(user);
  strncpy(dest, cwd+6+i);
  printf("\n%s@%s:~%s$",user ,host, dest);
}


void executeBuiltInCommand(char* cmd);
void executeCommand(char* cmd);
bool isBackgroundJob(char* cmd);
void waitpid(int childPid);


int main(){
  int flag = 0;
  char cmdLine[MAX];
  while(1){

    int childPid;
    char * cmdLine;

    printPrompt();

    flag = readCommandLine(cmdLine); //or GNU readline("");
    if (flag){continue;}
    cmd = parseCommand(cmdLine);

    if ( isBuiltInCommand(cmd)){
      executeBuiltInCommand(cmd);
    } else {
      childPid = fork();
      if (childPid == 0){
        executeCommand(cmd); //calls execvp

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
