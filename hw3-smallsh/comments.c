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

volatile sig_atomic_t ctrlZBackground = 0; // global variable for determining whether its currently foreground or non-foreground mode 

struct inOut // creating a struct that organizes parse information like:
{
    char inputFile[10001]; // if there's an input file, store it here
    char outputFile[10001]; // if there's an output file, store it here
    int background; // if there's an ampersand at the end, change background int to 1
};

struct inOut initialize() // constructor that initializes the 
{
    struct inOut tuple = {"\0", "\0", 0}; // null initialization 
    return tuple;
}
/*
Specifically for this portion, I was inspired by this stack overflow page: https://stackoverflow.com/questions/8137244/best-way-to-replace-a-part-of-string-by-another-in-c
*/
void replacePid(char *target, const char *key, const char *replacement) // this function is specifically for replacing every $$ occurence in the terminal with its respective PID
// Target is the input from the user, key is (in this case $$) the thing we want to replace, and replacement is what we will replace it with.
{
    char buffer[1024] = {0}; // this buffer function is where the modified string will be stored
    char *insert_point = &buffer[0]; // pointer to the buffer
    const char *tmp = target; // this is a pointer to the target array 
    size_t key_len = strlen(key); // this is the length of the key
    size_t repl_len = strlen(replacement); // this is the replacement length

    while(1) 
    {
        char *p = strstr(tmp, key); // checking base case if they are the same
        if (p == NULL) // if value is null
        {
            strcpy(insert_point, tmp); // then just copy over
            break;
        }

        memcpy(insert_point, tmp, p - tmp); // otherwise, add some space, and insert the string required
        insert_point += p - tmp;

        memcpy(insert_point, replacement, repl_len); // similarly do the same for replacement
        insert_point += repl_len;

        tmp = p + key_len;
    }
    strcpy(target, buffer); // copy the array to the target, from the buffer
}

char *shellUI() // this function takes the input of the user
{
    char command[10000]; // really large input amount just to be sure
    printf(": "); // prompt the user input
    fflush(stdout); // flush
    fgets(command, 100000, stdin); // take the user input
    if (strlen(command) > 0 && command[strlen(command) - 1] == '\n') 
    {
        command[strlen(command)-1] = '\0'; // add an endline, so that when looping through there's termination
    }
    // printf("%s\n", command);
    return command; // return that array
}

char **parseInput(char *command) // time to parse the input from the UI
{
    char commandParse[10000]; // take the input and strcpy it
    int counter = 0; // counter for num of words 
    char *fragmentArray[512]; // fragment array stores every argument into an array, each with its own index
    strcpy(commandParse, command); // function to strcpy

    char *saveptr; // saveptr just for strtok_r
    char *token = strtok_r(commandParse, " ", &saveptr); // initial parse
	while(token != NULL)
	{
        fragmentArray[counter] = token; // continue parsing
		token = strtok_r(NULL, " ", &saveptr);
        counter++;
        
	}
    fragmentArray[counter] = "\0"; // add a final element to terminate loops on

    return fragmentArray; // return parsed input
}

struct inOut parseSpecialInput(char **fragmentArray) // this function specifically looks out for < > and &
{
    int i = 0; // counter
    char inputFile[10001];
    char outputFile[10001];
    char* ch = fragmentArray[i]; // an instance of array
    struct inOut inOutValues = initialize(); // initialize struct that stores input output and background info
    while(strcmp(ch, "\0") != 0) // iterate through each fragment
    {
        if(strcmp(ch, "<") == 0) // check to see if it has a < character
        {
            snprintf(inOutValues.inputFile, sizeof(inOutValues.inputFile), "%s\0", fragmentArray[i+1]); // add input file to the right of it
        }
        else if(strcmp(ch, ">") == 0) // check to see if it has a > character
        {
            snprintf(inOutValues.outputFile, sizeof(inOutValues.outputFile), "%s\0", fragmentArray[i+1]); // add output file to the right of it
        }
        ch = fragmentArray[++i]; // iterate
    }     
    if(strcmp(fragmentArray[i-1], "&") == 0)
    {
        inOutValues.background = 1; // checking for ampersand at the end, if user wants background process
    }
    return inOutValues; // return tuple
}

void printParse(char **fragmentArray) // test printing function
{
    int i = 0;
    char* ch = fragmentArray[i];
    while(strcmp(ch, "\0") != 0) // iterate through
    {
        printf("%s\n", ch); // print array
        fflush(stdout);
        ch = fragmentArray[++i]; 
    }    

}

void execCommand(char **fragmentArray) // this function executes command using execvp
{
    int i = 0;
    char* ch = fragmentArray[i];
    while(strcmp(ch, "\0") != 0)
    {
        ch = fragmentArray[++i]; // iterate through the array
    }
    fragmentArray[i] = NULL; // make sure that you set the last element to null
    execvp(fragmentArray[0], fragmentArray); // run the command with args
    perror(""); // error handling
    printf("\n");
    fflush(stdout);
}


int cdFunc(char **fragmentArray) // this is the cd function
{
    int i = 0; // counter
    char* ch = fragmentArray[i]; // iterating through args
    while(strcmp(ch, "\0") != 0)
    {
        ch = fragmentArray[++i]; // iterating through args
    } 
    if(i == 1)
    {
        chdir(getenv("HOME")); // if there's no extra parameters, then go to home directory
    }
    else
    {
        if(chdir(fragmentArray[1]) == -1) // go to directory with filePath
        {
            printf("No such file or directory.\n"); // error message
            fflush(stdout);
        }
    }
}

int forcFunc(char **fragmentArray, struct inOut inOutValues, struct sigaction SIGINT_action) // this is a generic forc function for anything that's not cd, exit, or status
{
    pid_t spawnpid = -5; // initialize spawnpid
    int childStatus = 0; // childStatus
    int childPid;
    // If fork is successful, the value of spawnpid will be 0 in the child, the child's pid in the parent
    spawnpid = fork(); // fork the program

    switch(spawnpid)
    {
        case -1: // error handling case, will usually never happen
            perror("fork() failed!\n");
            exit(1);
            break;
        case 0: // child execution
            SIGINT_action.sa_handler = SIG_DFL; // recieve signals, if any
            sigaction(SIGINT, &SIGINT_action, NULL); // make sure the Ctrl C function works normally during the fork, as it's disabled in main
            if(strcmp(inOutValues.outputFile, "\0") != 0) // if there's an output file
            {
                int targetFD = open(inOutValues.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if (targetFD == -1) 
                    {
                        perror("open()\n");
                        exit(1);
                    }
                // Currently printf writes to the terminal
                // Use dup2 to point FD 1, i.e., standard output to targetFD
                int result = dup2(targetFD, 1); // use dup2 to make sure that the file is written into
                if (result == -1) 
                {
                    perror("dup2\n"); 
                    exit(2); 
                }
                // Now whatever we write to standard out will be written to targetFD
                fragmentArray[1] = "\0"; // make sure to remove the characters
            }
            if(strcmp(inOutValues.inputFile, "\0") != 0) // if there's an input file
            {
                int targetFD = open(inOutValues.inputFile, O_RDONLY, 0640);
                if (targetFD == -1) 
                    {
                        perror("open()\n");
                        exit(1);
                    }
                // Currently printf writes to the terminal
                // Use dup2 to point FD 1, i.e., standard output to targetFD
                int result = dup2(targetFD, 0); // use dup2 to make sure the file is input into stdin
                if (result == -1) 
                {
                    perror("dup2\n"); 
                    exit(2); 
                }
                // Now whatever we write to standard out will be written to targetFD
                fragmentArray[1] = "\0"; // make sure to remove characters
            }

            if(inOutValues.background) // if user wants background process
            {
                int i = 0;
                char* ch = fragmentArray[i];
                while(strcmp(ch, "\0") != 0)
                {
                    ch = fragmentArray[++i];
                }
                fragmentArray[i-1] = "\0"; // make sure to parse out the command before exec                 
            }      

            execCommand(fragmentArray); //exec
            exit(1);
            break;
        default:
            if(inOutValues.background == 0) // if foreground process
            {
                childPid = waitpid(spawnpid, &childStatus, 0); // wait for process to end
            }
            else if(inOutValues.background == 1)
            {

                if(ctrlZBackground == 0) // if exited from foreground-only
                {
                    printf("background pid is %d\n", spawnpid); // let it run in the background, no need to wait for process
                    fflush(stdout);
                }
                
                else if(ctrlZBackground == 1) // if forced into foreground-only
                {
                    childPid = waitpid(spawnpid, &childStatus, 0); // wait for process to end
                }
                
            }
        pid_t result; // result variable to output the process into
        int printStatus = 0; // will be used as an ID for the signal process.
        while((result = waitpid(-1, &printStatus, WNOHANG)) > 0) // this line is currently looking for non-terminated processes that are running in the background
        {
            fflush(stdout);
            if(WIFSIGNALED(printStatus)) // if a signal was terminated abnormally
            {
                printf("background process %d terminated with signal %d.\n", result, WTERMSIG(printStatus)); // return the corresponding ID for that abnormal termination, as well as the signal
                fflush(stdout); 
            }
            else
            {
                printf("background process %d exited with signal %d. \n", result, WEXITSTATUS(printStatus)); // otherwise, return the normal exit status
                fflush(stdout);
            }
        }

    }

    return childStatus;
}

void handle_SIGTSTP(int signo){ // this function handles the Ctrl Z functionality for this program
	char* message1; // this is the first message output to stdin
    char* message2; // this is the second message output to stdin
    if(ctrlZBackground == 0) // if currently not in foreground-only mode
    {
        message1 = "\nentering foreground-only mode\n"; // craft message
        ctrlZBackground = 1; // enter foreground-only mode
        write(1, message1, 31); // write to stdin
        fflush(stdout); // flush
    }
    else if(ctrlZBackground == 1) // if currently in foreground-only mode
    {
        message2 = "\nexiting foreground-only mode\n"; // craft message
        ctrlZBackground = 0; // exit foreground-only mode
        write(1, message2, 30); // write to stdin
        fflush(stdout); // flush
    }
}

int main(int argc, char *argv[])
{
    // printf("THIS IS THE PID IN MAIN %d", getpid());

    int status = 0;
    // Initialize SIGINT_action struct to be empty
    struct sigaction SIGINT_action = {0};
    struct sigaction SIGTSTP_action = {0};
    // Fill out the SIGINT_action struct
    // Register handle_SIGINT as the signal handler
    SIGINT_action.sa_handler = SIG_IGN;
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    // Block all catchable signals while handle_SIGINT is running
    sigfillset(&SIGINT_action.sa_mask);
    sigfillset(&SIGTSTP_action.sa_mask);
    // No flags set
    SIGINT_action.sa_flags = 0;
    SIGTSTP_action.sa_flags = SA_RESTART;

    // Install our signal handler
    sigaction(SIGINT, &SIGINT_action, NULL);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    fflush(stdout);
    while(1)
    {
        char **fragmentArray = parseInput(shellUI()); // parse the input
        struct inOut inOutValues; // create the tuple struct for organizing special character info
        inOutValues = parseSpecialInput(fragmentArray); // parse the special character info
        int i = 0; // counter
        char* ch = fragmentArray[i]; // first element of the input
        int pidCount = 0; // pidCount is initialized to 0
        char pidValue[10000]; // the actual value of the pid
        int wstatus = 0; // global (in main) status variable

        while(strcmp(ch, "\0") != 0) // echo $$ filtering
        {
            snprintf(pidValue, sizeof(pidValue), "%d", getpid());
            replacePid(ch, "$$", pidValue); // iterate and replace
            ch = fragmentArray[++i];
        }

        if(strcmp(fragmentArray[0], "exit") == 0)
        {
            break; // exit program if user inputs exit
        }
        else if(strcmp(fragmentArray[0], "cd") == 0)
        {
            cdFunc(fragmentArray); // cd program if user inputs cd
        }
        else if(strcmp(fragmentArray[0], "status") == 0)
        {
            if(WIFSIGNALED(wstatus)) // similar to earlier, where either terminated process, else naturally exiting process
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
        else if((fragmentArray[0][0] == '#') != 0) //ignore comments
        {
            ;
        }
        else
        {
            wstatus = forcFunc(fragmentArray, inOutValues, SIGINT_action); // fork if not a built in function
        }
    }
}