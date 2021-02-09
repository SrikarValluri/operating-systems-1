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
#include <errno.h>

volatile sig_atomic_t ctrlZBackground = 0;

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
    // printf("%s\n", command);
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
        }
        else if(strcmp(ch, ">") == 0)
        {
            snprintf(inOutValues.outputFile, sizeof(inOutValues.outputFile), "%s\0", fragmentArray[i+1]);
        }
        ch = fragmentArray[++i];
    }     
    if(strcmp(fragmentArray[i-1], "&") == 0)
    {
        inOutValues.background = 1;
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
        fflush(stdout);
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
    perror("");
    printf("\n");
    fflush(stdout);
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
            printf("No such file or directory.\n");
            fflush(stdout);
            return(1);
        }
        return(0);
    }
}

int forcFunc(char **fragmentArray, struct inOut inOutValues, struct sigaction SIGINT_action)
{
    int status = 0;
    pid_t spawnpid = -5;
    int childStatus = 0;
    int childPid;
    // If fork is successful, the value of spawnpid will be 0 in the child, the child's pid in the parent
    spawnpid = fork();

    switch(spawnpid)
    {
        case -1:
            perror("fork() failed!\n");
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
            exit(1);
            break;
        default:
            if(inOutValues.background == 0)
            {
                childPid = waitpid(spawnpid, &childStatus, 0);
                // if(WIFSIGNALED(childStatus) != 0)
                // {
                //     printf("Terminated by signal: %d \n", WTERMSIG(childStatus));
                //     fflush(stdout);
                // }
                // break;
            }
            else if(inOutValues.background == 1)
            {

                if(ctrlZBackground == 0)
                {
                    printf("background pid is %d\n", spawnpid);
                    fflush(stdout);
                }
                
                else if(ctrlZBackground == 1)
                {
                    childPid = waitpid(spawnpid, &childStatus, 0);
                }
                
            }
        pid_t result;
        int printStatus = 0;
        while((result = waitpid(-1, &printStatus, WNOHANG)) > 0)
        {
            fflush(stdout);
            if(WIFSIGNALED(printStatus))
            {
                printf("background process %d terminated with signal %d.\n", result, WTERMSIG(printStatus));
                fflush(stdout);
            }
            else
            {
                printf("background process %d exited with status %d. \n", result, WEXITSTATUS(printStatus));
                fflush(stdout);
            }
        }

    }

    return childStatus;
}

void handle_SIGTSTP(int signo){
	char* message1;
    char* message2;
    if(ctrlZBackground == 0)
    {
        message1 = "\nforeground mode\n";
        ctrlZBackground = 1;
        // printf("hel;lo1111");
        write(1, message1, 17);
        fflush(stdout);

        // printf("%s\n", message1);
    }
    else if(ctrlZBackground == 1)
    {
        message2 = "\nnon-foreground mode\n";
        // printf("helloooooooooooo\n");
        ctrlZBackground = 0;
        // printf("%s\n", message2);
        write(1, message2, 21);
        fflush(stdout);
    }
}

int main(int argc, char *argv[])
{
    // printf("THIS IS THE PID IN MAIN %d", getpid());

    int status = 0;
    // Initialize SIGINT_action struct to be empty
    struct sigaction SIGINT_action = {0};
    struct sigaction SIGTSTP_action = {0};
    struct sigaction ignore_action = {0};
    // Fill out the SIGINT_action struct
    // Register handle_SIGINT as the signal handler
    SIGINT_action.sa_handler = SIG_IGN;
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    ignore_action.sa_handler = SIG_IGN;
    // Block all catchable signals while handle_SIGINT is running
    sigfillset(&SIGINT_action.sa_mask);
    sigfillset(&SIGTSTP_action.sa_mask);
    // No flags set
    SIGINT_action.sa_flags = 0;
    SIGTSTP_action.sa_flags = SA_RESTART;

    // Install our signal handler
    sigaction(SIGINT, &SIGINT_action, NULL);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    sigaction(SIGTERM, &ignore_action, NULL);
	sigaction(SIGHUP, &ignore_action, NULL);
	sigaction(SIGQUIT, &ignore_action, NULL);


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
        int wstatus;

        while(strcmp(ch, "\0") != 0)
        {
            snprintf(pidValue, sizeof(pidValue), "%d", getpid());
            replacePid(ch, "$$", pidValue);
            ch = fragmentArray[++i];
        }

        if(strcmp(fragmentArray[0], "exit") == 0)
        {
            // status = 0;
            // printf("Last process exited with status %d.\n", status);
            break;
        }
        else if(strcmp(fragmentArray[0], "cd") == 0)
        {
            status = cdFunc(fragmentArray);
        }
        else if(strcmp(fragmentArray[0], "status") == 0)
        {
            if(WIFSIGNALED(wstatus))
            {
                printf("exit status %d.\n", WTERMSIG(wstatus));
                fflush(stdout);
            }
            else
            {
                printf("exit status %d. \n", WEXITSTATUS(wstatus));
                fflush(stdout);
            }

        }
        else if((fragmentArray[0][0] == '#') != 0)
        {
            status = 0;
        }
        else
        {
            wstatus = forcFunc(fragmentArray, inOutValues, SIGINT_action);
   
    
        }
    }
}

        // while(spawnpid > 0)
        // {
        //     printf("background process complete: %d\n", spawnpid);
        //     if(WIFEXITED(childStatus))
        //     {
        //         printf("exit status %d\n", WEXITSTATUS(childStatus));
        //         fflush(stdout);
        //     }
        //     else
        //     {
        //         printf("Terminating signal: %d\n", childStatus);
        //         fflush(stdout);
        //     }
        //     spawnpid = waitpid(-1, &childStatus, WNOHANG);
        // }