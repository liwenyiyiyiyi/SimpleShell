#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <libgen.h>
#include "parse.c"


#define MAX 1024
#define MAXARG 64
#define MAXPIPE 32




typedef struct job{
    pid_t pid;
    int job_id;

    /*location + -  */
    char location;

    /*status Done Terminated Running*/
    char* status;
    char cmd[MAX];
};


int jobnum = 0;
struct job* all_job;

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
  strncpy(dest, cwd+6+i,1024);
  printf("\n%s@%s:~%s$",user ,host, dest);
}


void executeBuiltInCommand(char** cmd){
  char cwd[MAX];

  /*case cd*/
  if (strcmp(cmd[0],"cd") == 0 ){

    /*case cd (i.e cd ~)*/
    if(cmd[1] == '\0'){
      cmd[1][0] = '~';
      cmd[1][1] = '\0';
      /*case cd ..*/
    }else if(strcmp(cmd[1],"..") == 0){

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
        printf("[%d]",all_job[i-1].job_id);
        /*TODO undefied all_job*/
        printf("%c  ",all_job[i-1].location);
        printf("%s",all_job[i-1].status);
        printf("              '%s'",all_job[i-1].cmd);

      }
      /*call by job_id*/
    }else{
      int i = 1;
      int jid = atoi(cmd[1]);
      /*TODO jobnum undefined*/
      for(;i < jobnum;i++){
        if(all_job[i-1].job_id == jid){
          prinf("[%d]",all_job[i-1].job_id);

          /*TODO undefied all_job*/
          printf("%c  ",all_job[i-1].location);
          printf("%s",all_job[i-1].status);
          printf("              '%s'",all_job[i-1].cmd);
          break;
        }
      }

    }

    /*case kill*/
  }else if (strcmp(cmd[0], "kill") == 0) {
      /*call by job_id*/
      if (cmd[1][0] == '%') {
          int i = 1;
          int length = strlen(cmd[1]);
          char* buf;
          int j = 1;
          for (;j < length;j++){
              buf[i-1] = cmd[1][i];
          }
          buf[length] = '\0';
          int jid = atoi(buf);
          for (; i < jobnum; i++) {
              if (all_job[i - 1].job_id == jid) {
                  all_job[i - 1].status = "Terminated";
                  break;

              }

          }

          /*call by pid*/
      } else {
          int i = 1;
          pid_t pid = atoi(cmd[1]);
          for (; i < jobnum; i++) {
              if (all_job[i - 1].pid == pid) {
                  all_job[i - 1].status = "Terminated";
                  break;
              }
          }
      }
  }
}

void executePiped(char *cmdLine)
{
    int counter = 0;
    int i = 0;
    int file[MAXPIPE][2];
    char *cmds[MAXARG];
    char *cmd[MAXARG];
    char tokenize = cmdLine[0];
    int j = 0;
    while (tokenize)
    {
        if (!strncmp(cmdLine,"|",1))
        {
            counter++;
        }
    }
    cmds = parseCommand(cmdLine,"|");

    for (; j<counter+1;j++)
    {
        int
        cmd = parseCommand(cmds[j]," ");
        if(!(i==counter))
        {
            pipe(file[j]);
        }
        if(!fork())
        {
            if(!(j==counter))
            {
                dup2(file[j][1],1);
				close(file[j][0]);
				close(file[j][1]);
            }
            if(j){
				dup2(file[j-1][0],0);
				close(file[j-1][1]);
				close(file[j-1][0]);
			}
            redirectionCommand(cmd);
            execvp(cmd[0],cmd);
        }
        if(j)
        {
            close(file[j-1][0]);
			close(file[j-1][1]);
        }
        wait(NULL);
    }
}

void executeCommand(char** cmd){
  char** new_command = redirectionCommand(cmd);
  execvp(new_command[0],new_command);
}


int isBackgroundJob(char* cmd){
      return 1;

}



int main(){
  int flag = 0;
  char cmdLine[MAX];
  char *cmd[MAXARG];
  while(1){

    int childPid;

    printPrompt();

    flag = readCommandLine(cmdLine); /*or GNU readline("");*/
    if (flag){continue;}
    if (strchr(cmdLine, '|'))
        {
            executePiped(cmdLine);
        }
    else
    {
    cmd = parseCommand(cmdLine," ");
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
          /*waitpid (childPid);*/
        }
      }
    }
  }


  }
}
