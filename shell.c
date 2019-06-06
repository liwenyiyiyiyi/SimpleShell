#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <libgen.h>
#include "parse.c"

#define MAX 1024

struct job{
    pid_t pid;
    int job_id;

    /*location + -  */
    char location;

    /*status Done Terminated Running*/
    char* status;
    char cmd[MAX];
};



void printPrompt(){
  char cwd[1024];
  char host[1024];
  char *user;
  char *env = "USER";
  char dest[1024];
  int i;
  user = getenv(env);
  gethostname(host,sizeof(host));
  getcwd(cwd, sizeof(cwd));
  i = strlen(user);
  strncpy(dest, cwd+6+i);
  printf("\n%s@%s:~%s$",user ,host, dest);
}


void executeBuiltInCommand(char** cmd){
  char cwd[1024];

  /*case cd*/
  if (strcmp(cmd[0],"cd") == 0 ){


    /*case cd (i.e cd ~)*/
    if(cmd[1] == '\0'){
      cmd[1][0] = '~';
      cmd[1][1] = '\0';
      cmd[1] == "~";
      /*case cd ..*/
    }else if(cmd[1] = '..'){

      getcwd(cwd, sizeof(cwd));
      cmd[1] = dirname(cwd);
    }
    chdir(cmd[1]);

    /*case exit*/
  }else if (strcmp(cmd[0],"exit") == 0){
    exit(0);

    /*case jobs*/
  }else if (strcmp(cmd[0],"jobs") == 0){


    /*print all jobs*/
    if (cmd[1] == NULL){
      int i = 1;
      /*TODO jobnum undefined*/
      for(;i < jobnum;i++){
        prinf("[%d]",all_job[i-1] -> job_id);
        /*TODO undefied all_job*/
        printf("%c  ",all_job[i-1] -> location);
        printf("%s",all_job[i-1] -> status);
        printf("              '%s'",all_job[i-1] -> cmd);

      }
      /*call by job_id*/
    }else{
      int i = 1;
      int jid = atoi(cmd[1][0]);
      /*TODO jobnum undefined*/
      for(;i < jobnum;i++){
        if(all_job[i-1] -> job_id == jid){
          prinf("[%d]",all_job[i-1] -> job_id);

          /*TODO undefied all_job*/
          printf("%c  ",all_job[i-1] -> location);
          printf("%s",all_job[i-1] -> status);
          printf("              '%s'",all_job[i-1] -> cmd);
          break;
        }
      }

    }

    /*case kill*/
  }else if (strcmp(cmd[0], "kill") == 0){
    /*call by job_id*/
    if(cmd[1][0] == '%'){
      int i = i;
      int jid = atoi(cmd[1][1]);
      for (;i < jobnum;i++){
        if (all_job[i-1] -> job_id == jid){
          all_job[i-1] -> status = "Terminated";
          break;

        }

      }

      /*call by pid*/
    }else{
      int i = 1;
      pid_t pid = atoi(cmd[1][0]);
      for (;i < jobnum;i++){
        if(all_job[i-1] -> pid == pid){
          all_job[i-1] -> status = "Terminated";
          break;
        }
      }
    }
}



void executeCommand(char** cmd){
  char** new_command = redirectionCommand(cmd);
  execvp(new_command[0],new_command);
}


bool isBackgroundJob(char* cmd);



int main(){
  int flag = 0;
  char cmdLine[MAX];
  while(1){

    int childPid;
    char** cmdLine;
    char** cmd;
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
