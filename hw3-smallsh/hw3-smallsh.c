#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <fcntl.h>


struct inOut 
{
    char inputFile[10001];
    char outputFile[10001];
    int background;
};

struct inOut initialize()
{
    struct inOut tuple = {"\0", "\0", 0};
    return tuple;
}

char *shellUI()
{
    char command[10000];
    printf(": ");
    fflush(stdout);
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

struct inOut parseSpecialInput(char **fragmentArray)
{
    int i = 0;
    char* ch = fragmentArray[i];
    char inputFile[10001];
    char outputFile[10001];
    struct inOut inOutValues = initialize();
    while(strcmp(ch, "\0") != 0)
    {
        if(strcmp(ch, "<") == 0)
        {
            snprintf(inOutValues.inputFile, sizeof(inOutValues.inputFile), "%s\0", fragmentArray[i+1]);
            printf("\n%s\n", inOutValues.inputFile);
        }
        else if(strcmp(ch, ">") == 0)
        {
            snprintf(inOutValues.outputFile, sizeof(inOutValues.outputFile), "%s\0", fragmentArray[i+1]);
            printf("\n%s\n", inOutValues.outputFile);
        }
        ch = fragmentArray[++i];
    }     
    if(strcmp(fragmentArray[i-1], "&") == 0)
    {
        inOutValues.background = 0;
    }
    return inOutValues;
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
    int i = 0;
    char* ch = fragmentArray[i];
    while(strcmp(ch, "\0") != 0)
    {
        printf("hi");
        ch = fragmentArray[++i];
    }
    fragmentArray[i] = NULL;
    execvp(fragmentArray[0], fragmentArray);
    perror("execvp");
}


int cdFunc(char **fragmentArray)
{
    int i = 0;
    char* ch = fragmentArray[i];
    while(strcmp(ch, "\0") != 0)
    {
        ch = fragmentArray[++i];
    } 
    if(i == 1)
    {
        chdir(getenv("HOME"));
        return(0);
    }
    else
    {
        if(chdir(fragmentArray[1]) == -1)
        {
            printf("bash: cd: %s: No such file or directory.\n", fragmentArray[1]);
            fflush(stdout);
            return(-1);
        }
        return(0);
    }
}

// char **outputIO(char **fragmentArray)
// {
//     // if(strcmp(inOutValues.outputFile, "\0") != 0)
//     // {
//     //     int targetFD = open(inOutValues.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
//     //     if (targetFD == -1) 
//     //         {
//     //             perror("open()");
//     //             exit(1);
//     //         }
//     //     // Currently printf writes to the terminal
//     //     printf("The file descriptor for targetFD is %d\n", targetFD);

//     //     // Use dup2 to point FD 1, i.e., standard output to targetFD
//     //     int result = dup2(targetFD, 1);
//     //     if (result == -1) 
//     //     {
//     //         perror("dup2"); 
//     //         exit(2); 
//     //     }
//     //     // Now whatever we write to standard out will be written to targetFD
//     //     fragmentArray[1] = "\0";
//     // }
//     return fragmentArray; 
// }

int forcFunc(char **fragmentArray, struct inOut inOutValues)
{
    int status = 0;
    pid_t spawnpid = -5;
    int childStatus;
    int childPid;
    // If fork is successful, the value of spawnpid will be 0 in the child, the child's pid in the parent
    spawnpid = fork();
    switch(spawnpid)
    {
        case -1:
            perror("fork() failed!\n");
            status = 1;
            exit(1);
            break;
        case 0:
            if(strcmp(inOutValues.outputFile, "\0") != 0)
            {
                int targetFD = open(inOutValues.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if (targetFD == -1) 
                    {
                        perror("open()");
                        exit(1);
                    }
                // Currently printf writes to the terminal
                printf("The file descriptor for targetFD is %d\n", targetFD);

                // Use dup2 to point FD 1, i.e., standard output to targetFD
                int result = dup2(targetFD, 1);
                if (result == -1) 
                {
                    perror("dup2"); 
                    exit(2); 
                }
                // Now whatever we write to standard out will be written to targetFD
                fragmentArray[1] = "\0";
            }
            execCommand(fragmentArray);
            status = 1;
            exit(1);
            break;
        default:
            childPid = waitpid(-1, &childStatus, 0);
            break;
    }
    return status;
}

int main(int argc, char *argv[])
{
    int status = 0;
    while(1)
    {
        char **fragmentArray = parseInput(shellUI());
        struct inOut inOutValues;
        inOutValues = parseSpecialInput(fragmentArray);
        // // printf("%s\n", inOutValues.inputFile); 
        // // if(strcmp(inOutValues.outputFile, "\0") != 0)
        // // {
        // //     printf("hiorigjoikg");
        // // }

        if(strcmp(fragmentArray[0], "exit") == 0)
        {
            status = 0;
            printf("Last process exited with status %d.\n", status);
            break;
        }
        else if(strcmp(fragmentArray[0], "cd") == 0)
        {
            status = cdFunc(fragmentArray);
        }
        else if(strcmp(fragmentArray[0], "status") == 0)
        {
            if(WIFEXITED(status))
            {
                printf("WIFStatus %d\n", WIFEXITED(status));
            }
            else
            {
                printf("Status %d\n", status);
            }
        }
        else if(strcmp(fragmentArray[0], "#") == 0)
        {
            ; //stuffff 
        }
        else
        {
            status = forcFunc(fragmentArray, inOutValues);
    
        }
    }
}

