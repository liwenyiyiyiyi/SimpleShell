#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <libgen.h>
#include "parse.h"

#define MAX 1024
#define MAXARG 64
#define MAXPIPE 32

typedef struct job
{
    pid_t pid;
    int job_id;

    /*location + -  */
    char location;

    /*status Done Terminated Running*/
    char *status;
    char cmd[MAX];
} job;

int jobnum = 0;
job all_job[MAXARG];

void printPrompt()
{
    char cwd[MAX];
    char host[MAX];

    char *user;
    char *env = "USER";
    char dest[MAX];
    int i;
    user = getenv(env);
    gethostname(host, sizeof(host));
    getcwd(cwd, sizeof(cwd));
    i = strlen(user);
    strncpy(dest, cwd + 6 + i, 1024);
    printf("%s@%s:~%s$", user, host, dest);
}

void executeBuiltInCommand(char *cmd[])
{
    char cwd[MAX];

    /*case cd*/
    if (strcmp(cmd[0], "cd") == 0)
    {

        /*case cd (i.e cd ~)*/
        if (cmd[1] == '\0')
        {
            cmd[1][0] = '~';
            cmd[1][1] = '\0';
            /*case cd ..*/
        }
        else if (strcmp(cmd[1], "..") == 0)
        {

            getcwd(cwd, sizeof(cwd));
            cmd[1] = dirname(cwd);
        }
        chdir(cmd[1]);

        /*case exit*/
    }
    else if (strcmp(cmd[0], "exit") == 0)
    {
        exit(0);

        /*case jobs*/
    }
    else if (strcmp(cmd[0], "jobs") == 0)
    {

        /*print all jobs*/
        if (cmd[1] == NULL)
        {
            printf("print all\n");
            int i = 1;
            /*TODO jobnum undefined*/
            for (; i < jobnum; i++)
            {
                printf("[%d]", all_job[i - 1].job_id);
                /*TODO undefied all_job*/
                printf("%c  ", all_job[i - 1].location);
                printf("%s", all_job[i - 1].status);
                printf("              '%s'", all_job[i - 1].cmd);
            }
            /*call by job_id*/
        }
        else
        {
            int i = 1;
            int jid = atoi(cmd[1]);
            /*TODO jobnum undefined*/
            for (; i < jobnum; i++)
            {
                if (all_job[i - 1].job_id == jid)
                {
                    printf("[%d]", all_job[i - 1].job_id);

                    /*TODO undefied all_job*/
                    printf("%c  ", all_job[i - 1].location);
                    printf("%s", all_job[i - 1].status);
                    printf("              '%s'", all_job[i - 1].cmd);
                    break;
                }
            }
        }

        /*case kill*/
    }
    else if (strcmp(cmd[0], "kill") == 0)
    {
        /*call by job_id*/
        if (cmd[1][0] == '%')
        {
            int i = 1;
            int length = strlen(cmd[1]);
            char *buf;
            int j = 1;
            int jid;
            for (; j < length; j++)
            {
                buf[i - 1] = cmd[1][i];
            }
            buf[length] = '\0';
            jid = atoi(buf);
            for (; i < jobnum; i++)
            {
                if (all_job[i - 1].job_id == jid)
                {
                    all_job[i - 1].status = "Terminated";
                    break;
                }
            }

            /*call by pid*/
        }
        else
        {
            int i = 1;
            pid_t pid = atoi(cmd[1]);
            for (; i < jobnum; i++)
            {
                if (all_job[i - 1].pid == pid)
                {
                    all_job[i - 1].status = "Terminated";
                    break;
                }
            }
        }
    }
}

void executePiped(char *command)
{
    int counter = 0;
    int i = 0;
    int file[MAXPIPE][2];
    char *cmds[MAXARG];
    char tokenize = command[i];
    int j = 0;
    /*printf("enter piped!\n");*/
    while (tokenize)
    {
        if (tokenize == '|')
        {
            counter++;
        }
        tokenize = command[++i];
    }

    /*use cmds to record simple commands*/
    parseCommand(command, "|", cmds);

    /*printf ("cmds[0] = %s ,   cmds[1] = %s\n",cmds[0],cmds[1]);*/
    for (; j < counter + 1; j++)
    {
        char *cmd[MAXARG];
        /*use cmd to record single command*/
        /*printf("\ncmd[j]: %s",cmds[j]);*/
        parseCommand(cmds[j], " ", cmd);
        /*printf("\ncmd %s %s",cmd[0],cmd[1]);*/
        if (j != counter)
        {
            pipe(file[j]);
        }
        if (fork() == 0)
        {
            if (j != counter)
            {
                dup2(file[j][1], 1);
                close(file[j][0]);
                close(file[j][1]);
            }
            if (j)
            {
                dup2(file[j - 1][0], 0);
                close(file[j - 1][1]);
                close(file[j - 1][0]);
            }
            redirectionCommand(cmd);
            execvp(cmd[0], cmd);
            /*TODO syntax error*/
            /*execvp(cmd[0],cmd);*/
        }
        if (j)
        {
            close(file[j - 1][0]);
            close(file[j - 1][1]);
        }
        wait(NULL);
    }
}

void executeCommand(char *cmd[])
{
    /*printf("execute\n");*/
    char **new_command = redirectionCommand(cmd);
    /*printf("new command = %s\n" , new_command[0]);*/

    execvp(new_command[0], new_command);
}

int isBackgroundJob(char **cmd)
{
    int k = 0;
    int flag = 0;
    while (cmd[k])
    {
        if (!strncmp(cmd[k], "&", 1))
        {
            cmd[k] = NULL;
            flag = 1;
        }
        k++;
    }
    return flag;
}
void jobs_initialize(int i)
{
    all_job[i].status = "empty";
    all_job[i].pid = 0;
    all_job[i].location = '1';
    all_job[i].job_id = 0;
}
void recordBackgroundJob(char **cmd, char *cmdLine)
{
    int i = 0;
    for (; i < MAXARG; i++)
    {
        if (all_job[i].status != "Running" && all_job[i].status != "Terminated")
        {
            break;
        }
    }
    all_job[i].job_id = i + 1;
    all_job[i].location = '+';
    if (i != 0)
    {
        all_job[i - 1].location = '-';
        if (i != 1)
        {
            all_job[i].location = ' ';
        }
    }

    all_job[i].pid = getpid();
    all_job[i].status = "Running";
    strcpy(all_job[i].cmd, cmdLine);
    jobs_initialize(i + 1);
    printf("[%d] %d\n", all_job[i].job_id, all_job[i].pid);
}

int main(int argc, char *argv[])
{
    char cmdLine[MAX];
    int childPid;
    FILE *fptr = fopen(argv[1], "r");
    jobs_initialize(0);
    /*l = fscanf(fptr, "%s", cmdLine);*/
    /*printf("hello\n");*/
    while (fgets(cmdLine, MAX, fptr) != NULL)
    {
        char *cmd[MAXARG];
        char buf[MAX];
        int f = 0;
        /*printf("read : cmdLine = %s\n", cmdLine);*/

        if (cmdLine[strlen(cmdLine) - 1] == '\n')
        {
            cmdLine[strlen(cmdLine) - 1] = '\0';
        }

        if (strlen(cmdLine) == 0)
        {
            continue;
        }
        if (strchr(cmdLine, '|'))
        {
            /*piped*/
            executePiped(cmdLine);

            /*simple not piped*/
        }
        else
        {
            /*not piped*/
            /*printf("not piped!\n");*/
            /*cmd is the command after parsing*/
            strcpy(buf,cmdLine);
            parseCommand(buf, " ", cmd);
            /*printf("end parse!\n");*/
            if (isBuiltInCommand(cmd))
            {
                /*printf("is builtin!\n");*/
                executeBuiltInCommand(cmd);
            }
            else
            {
                /*printf("not builtin\n");*/
                f = isBackgroundJob(cmd);
                childPid = fork();
                if (childPid == 0 && !f)
                {
                    /*printf("execute\n");*/
                    redirectionCommand(cmd);
                    /*printf("here     cmd[0] = %s\n",cmd[0]);*/
                    execvp(cmd[0], cmd); /*calls execvp*/
                }
                else
                {
                    if (f && childPid == 0)
                    {
                        recordBackgroundJob(cmd,cmdLine);
                        /*record in list of background jobs*/
                    }
                    else
                    {
                        waitpid(childPid,NULL,0);
                        /*waitpid (childPid);*/
                    }
                }
            }
        }
    }
    fclose(fptr);
    exit(0);
}
