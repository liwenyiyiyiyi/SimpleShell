#include "parse.h"
char* readCommandLine(char *cmdLine)
{
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
