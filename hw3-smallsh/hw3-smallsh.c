#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <fcntl.h>




char *shellUI()
{
    char command[10000];
    printf(": ");
    fgets(command, 100000, stdin);
    if (strlen(command) > 0 && command[strlen(command) - 1] == '\n') 
    {
        command[strlen(command)-1] = '\0';
    }
    return command;
}

char **parseInput(char *command)
{
    char commandParse[10000];
    int counter = 0;
    char *fragmentArray[512];
    strcpy(commandParse, command);

    char *saveptr;
    char *token = strtok_r(commandParse, " ", &saveptr);
	while(token != NULL)
	{
        fragmentArray[counter] = token;
		token = strtok_r(NULL, " ", &saveptr);
        counter++;
        
	}
    fragmentArray[counter] = "\0";

    return fragmentArray;
}

void printParse(char **fragmentArray)
{
    int i = 0;
    char* ch = fragmentArray[i];
    while(strcmp(ch, "\0") != 0)
    {
        printf("%s\n", ch);
        ch = fragmentArray[++i];
    }    

}

void execCommand(char **fragmentArray)
{
    pid_t spawnpid = -5;
    int childStatus;
    int childPid;

    int i = 0;
    char* ch = fragmentArray[i];
    while(strcmp(ch, "\0") != 0)
    {
        printf("%s\n", ch);
        ch = fragmentArray[++i];
    }
    fragmentArray[i] = NULL;
    execvp(fragmentArray[0], fragmentArray);
    perror("execv");
}

// void exitFunc()
// {
    
// }

// void cdFunc()
// {
    
// }

// void statusFunc()
// {
    
// }

    // spawnpid = fork();
    // switch (spawnpid){
    // case -1:
    //     perror("fork() failed!");
    //     exit(1);
    // case 0:
    //     execvp(fragmentArray[0], fragmentArray);
    //     perror("execv");
    // default:
    // printf("I am the parent. My pid  = %d\n", getpid());
    // childPid = wait(&childStatus);
    //         printf("Parent's waiting is done as the child with pid %d exited\n", childPid);
    // }
int main(int argc, char *argv[])
{
    while(1)
    {
        char **fragmentArray = parseInput(shellUI());
        pid_t spawnpid = -5;
	    int childStatus;
        int childPid;
        // If fork is successful, the value of spawnpid will be 0 in the child, the child's pid in the parent
	    spawnpid = fork();
        switch (spawnpid)
        {
            case -1:
                perror("fork() failed!");
                exit(1);
                break;
            case 0:
                printf("I am the child. My pid  = %d\n", getpid());
                execCommand(fragmentArray);
            default:
                printf("I am the parent. My pid  = %d\n", getpid());
                childPid = wait(&childStatus);
                printf("Parent's waiting is done as the child with pid %d exited\n", childPid);
	    }
        printf("The process with pid %d is returning from main\n", getpid());
        return 0;
    }


        // if(fragmentArray[0] == "exit")
        // {
        //     printf('Process complete with exit code 0.');
        //     break;
        // }
        // if(fragmentArray[0] == "cd")
        // {
        //     cdFunc(fragmentArray)
        // }

    // execvp("ls", "ls");

}

