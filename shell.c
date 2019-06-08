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

void update_location()
{
    int current = 0;
    int counter = 0;
    int previous = -1;
    for (; current < jobnum; current++)
    {
        if (!strcmp(all_job[current].status, "Running") || !strcmp(all_job[current].status, "Done") || !strcmp(all_job[current].status, "Terminated"))
        {
            previous = counter;
            counter = current;
        }
        all_job[current].location = ' ';
    }
    if (previous != -1)
    {
        all_job[previous].location = '-';
    }
    all_job[counter].location = '+';
    jobnum = counter + 1;
}

int find_job(pid_t job_pid)
{
    int i = 0;
    for (; i < MAXARG; i++)
    {
        if (all_job[i].pid == job_pid)
        {
            return all_job[i].job_id;
        }
    }
    return 0;
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
            int i = 1;
            /*TODO jobnum undefined*/
            for (; i < jobnum+1; i++)
            {
                if (strcmp(all_job[i - 1].status, "Running") == 0)
                {
                    printf("[%d]", all_job[i - 1].job_id);
                    /*TODO undefied all_job*/
                    printf("%c  ", all_job[i - 1].location);
                    printf("%s", all_job[i - 1].status);
                    printf("              %s &\n", all_job[i - 1].cmd);
                }
                else if (strcmp(all_job[i - 1].status, "Done") == 0)
                {
                    printf("[%d]", all_job[i - 1].job_id);
                    /*TODO undefied all_job*/
                    printf("%c  ", all_job[i - 1].location);
                    printf("%s", all_job[i - 1].status);
                    printf("              %s\n", all_job[i - 1].cmd);

                    /*delete*/
                    all_job[i - 1].status = "Empty";
                }
                else if (strcmp(all_job[i - 1].status, "Terminated") == 0)
                {
                    printf("[%d]", all_job[i - 1].job_id);
                    /*TODO undefied all_job*/
                    printf("%c  ", all_job[i - 1].location);
                    printf("Running");
                    printf("              %s\n", all_job[i - 1].cmd);
                    all_job[i - 1].status = "Empty";
                }
                else
                {
                    continue;
                }
            }
            /*call by job_id*/
        }
        else
        {
            int i = 1;
            int jid = atoi(cmd[1]);
            /*TODO jobnum undefined*/
            for (; i < jobnum+1; i++)
            {
                if (all_job[i - 1].job_id == jid)
                {
                    printf("[%d]", all_job[i - 1].job_id);

                    /*TODO undefied all_job*/
                    printf("%c  ", all_job[i - 1].location);
                    if (strcmp(all_job[i - 1].status, "Running") == 0)
                    {
                        printf("Running");
                    }
                    else if (strcmp(all_job[i - 1].status, "Terminated") == 0)
                    {
                        printf("Running");
                        all_job[i - 1].status = "Empty";
                    }
                    else if (strcmp(all_job[i - 1].status, "Done") == 0)
                    {
                        printf("Done");
                        all_job[i - 1].status = "Empty";
                    }
                    printf("              %s\n", all_job[i - 1].cmd);
                }
                else
                {
                    if (strcmp(all_job[i - 1].status, "Terminated") == 0 || strcmp(all_job[i - 1].status, "Done") == 0)
                    {
                        all_job[i - 1].status = "Empty";
                    }
                }
            }
        }
        update_location();
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
            printf("jid = %d\n",jid);
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
    all_job[i].status = "Empty";
    all_job[i].pid = 0;
    all_job[i].location = ' ';
    all_job[i].job_id = 0;
}

void recordBackgroundJob(char **cmd, char *cmdLine)
{
    all_job[jobnum].job_id = jobnum + 1;
    all_job[jobnum].pid = getpid();
    all_job[jobnum].status = "Running";
    strcpy(all_job[jobnum].cmd, cmdLine);
    jobs_initialize(jobnum + 1);
    jobnum = jobnum + 1;
    update_location();
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
        int pid;
        int JobId;
        int status;
        /*printf("read : cmdLine = %s\n", cmdLine);*/

        /*TODO check and print Done Job*/
        /*0 -> running       1 -> Done     4 -> Terminated*/

        /*check whether there is job done*/
        pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);

        for (; pid > 0; pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED))
        {

            if (WIFEXITED(status))
            {

                /*search job with pid equal to pid and set the status to Done*/
                /*1. search suppose jobid is JobID*/
                /*2. set status*/
            }
        }
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
            strcpy(buf, cmdLine);
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
                if (childPid == 0)
                {
                    /*printf("execute\n");*/
                    redirectionCommand(cmd);
                    /*printf("here     cmd[0] = %s\n",cmd[0]);*/
                    execvp(cmd[0], cmd); /*calls execvp*/
                }
                else
                {
                    if (f)
                    {
                        int c = 0;
                        while (cmdLine[c])
                        {
                            if (cmdLine[c] == '&')
                            {
                                cmdLine[c - 1] = '\0';
                            }
                            c++;
                        }
                        recordBackgroundJob(cmd, cmdLine);
                        /*record in list of background jobs*/
                    }
                    else
                    {
                        waitpid(childPid, NULL, 0);
                        /*waitpid (childPid);*/
                    }
                }
            }
        }
    }
    fclose(fptr);
    exit(0);
}
