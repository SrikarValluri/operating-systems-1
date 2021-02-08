#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <fcntl.h>
#include <signal.h>



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

void replacePid(char *target, const char *needle, const char *replacement)
{
    char buffer[1024] = {0};
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) 
    {
        const char *p = strstr(tmp, needle);

        if (p == NULL) 
        {
            strcpy(insert_point, tmp);
            break;
        }

        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        tmp = p + needle_len;
    }
    strcpy(target, buffer);
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
        inOutValues.background = 1;
        printf("hi\n");
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
        ch = fragmentArray[++i];
    }
    fragmentArray[i] = NULL;
    execvp(fragmentArray[0], fragmentArray);
    perror("Invalid Input: \n");
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

int forcFunc(char **fragmentArray, struct inOut inOutValues, struct sigaction SIGINT_action)
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
            SIGINT_action.sa_handler = SIG_DFL;
            sigaction(SIGINT, &SIGINT_action, NULL);
            if(strcmp(inOutValues.outputFile, "\0") != 0)
            {
                int targetFD = open(inOutValues.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if (targetFD == -1) 
                    {
                        perror("open()\n");
                        exit(1);
                    }
                // Currently printf writes to the terminal
                // Use dup2 to point FD 1, i.e., standard output to targetFD
                int result = dup2(targetFD, 1);
                if (result == -1) 
                {
                    perror("dup2\n"); 
                    exit(2); 
                }
                // Now whatever we write to standard out will be written to targetFD
                fragmentArray[1] = "\0";
            }
            if(strcmp(inOutValues.inputFile, "\0") != 0)
            {
                int targetFD = open(inOutValues.inputFile, O_RDONLY, 0640);
                if (targetFD == -1) 
                    {
                        perror("open()\n");
                        exit(1);
                    }
                // Currently printf writes to the terminal
                printf("The file descriptor for targetFD is %d\n", targetFD);

                // Use dup2 to point FD 1, i.e., standard output to targetFD
                int result = dup2(targetFD, 0);
                if (result == -1) 
                {
                    perror("dup2\n"); 
                    exit(2); 
                }
                // Now whatever we write to standard out will be written to targetFD
                fragmentArray[1] = "\0";
            }

            if(inOutValues.background)
            {
                int i = 0;
                char* ch = fragmentArray[i];
                while(strcmp(ch, "\0") != 0)
                {
                    ch = fragmentArray[++i];
                }
                fragmentArray[i-1] = "\0";                 
            }      

            execCommand(fragmentArray);
            status = 1;
            exit(1);
            break;
        default:
            if(inOutValues.background == 0)
            {
                childPid = waitpid(-1, &childStatus, 0);
                break;
            }
            if(inOutValues.background == 1)
            {
                childPid = waitpid(-1, &childStatus, WNOHANG);              
                break;
            }
    }

    return status;
}

int main(int argc, char *argv[])
{
    int status = 0;
    // Initialize SIGINT_action struct to be empty
    struct sigaction SIGINT_action = {0};

    // Fill out the SIGINT_action struct
    // Register handle_SIGINT as the signal handler
    SIGINT_action.sa_handler = SIG_IGN;
    // Block all catchable signals while handle_SIGINT is running
    sigfillset(&SIGINT_action.sa_mask);
    // No flags set
    SIGINT_action.sa_flags = 0;

    // Install our signal handler
    sigaction(SIGINT, &SIGINT_action, NULL);

    // printf("Send the signal SIGINT to this process by entering Control-C. That will cause the signal handler to be invoked\n");
    fflush(stdout);
    while(1)
    {
        char **fragmentArray = parseInput(shellUI());
        struct inOut inOutValues;
        inOutValues = parseSpecialInput(fragmentArray);
        int i = 0;
        char* ch = fragmentArray[i];
        int pidCount = 0;
        char pidValue[10000];

        while(strcmp(ch, "\0") != 0)
        {
            snprintf(pidValue, sizeof(pidValue), "%d", getpid());
            replacePid(ch, "$$", pidValue);
            ch = fragmentArray[++i];
        }

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
            if(WIFSIGNALED(status))
            {
                printf("WIFStatus %d\n", WIFEXITED(status)); //WTERMSIG
            }
            else
            {
                printf("Status %d\n", status);
            }
        }
        else if((fragmentArray[0][0] == '#') != 0)
        {
            ;
        }
        else
        {
            status = forcFunc(fragmentArray, inOutValues, SIGINT_action);
    
        }
    }
}

