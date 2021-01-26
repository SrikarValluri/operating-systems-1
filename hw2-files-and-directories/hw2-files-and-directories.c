// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <fcntl.h>

/* struct for important movie information */
struct movie
{
    char *title;
    char *year;
    char *languages;
    char *rating;
    struct movie *next;
};

/*
* Print data for the given movie
*/
void printMovie(struct movie *aMovie)
{
    printf("%s, %s %s, %s\n", aMovie->title,
           aMovie->languages,
           aMovie->year,
           aMovie->rating);
}
/*
* Print the struct attributes of a specific movie
*/
void printMovieList(struct movie *list)
{
    while (list != NULL)
    {
        printMovie(list);
        list = list->next;
    }
}

/*
Print the entire linked list, and the attributes of each struct in the list
*/
int returnMovieCount(struct movie *list)
{
    int i = 0;
    while (list != NULL)
    {
        i++;
        list = list->next;
    }
    return i;
}

/* Parse the current line which is space delimited and create a
*  movie struct with the data in this line
*/
struct movie *createMovie(char *currLine)
{
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->year, token);

    // The next token is the languages
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, token);

    // The last token is the rating
    token = strtok_r(NULL, "\r\n", &saveptr);
    currMovie->rating = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->rating, token);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}

/*
* Return a linked list of movies by parsing data from
* each line of the specified file.
*/
struct movie *processFile(char *filePath)
{
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;
    int lineCounter = 0;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        if(lineCounter != 0)
        {
            // Get a new movie node corresponding to the current line
            struct movie *newNode = createMovie(currLine);


            // Is this the first node in the linked list?
            if (head == NULL)
            {
                // This is the first node in the linked link
                // Set the head and the tail to this node
                head = newNode;
                tail = newNode;
            }
            else
            {
                // This is not the first node.
                // Add this node to the list and advance the tail
                tail->next = newNode;
                tail = newNode;
            }
        }
        lineCounter++;
    }
    free(currLine);
    fclose(movieFile);
    return head;
}

void processFile2(char *filePath)
{
    struct movie *list = processFile(filePath);
    printf("Processed file %s and parsed data for %i movies\n", filePath, returnMovieCount(list)); //sending message to user to confirm success of linked list creation
    // printMovieList(list);
    char newDirectory[100];
    char newDirectoryTemp[100];


    srand(time(NULL));
    int randomNumber;
    randomNumber = rand() % 100000;

    struct stat st = {0};

    snprintf(newDirectory, 100, "./valluris.movies.%d", randomNumber);
    printf("Created directory with name valluris.movies.%d\n", randomNumber);

    if(stat(newDirectory, &st) == -1) 
    {
        mkdir(newDirectory, 0750);
    }
    while(list != NULL)
    {
        snprintf(newDirectoryTemp, 100, "%s/%s", newDirectory, list->year);
        int fileInt = open(newDirectoryTemp, O_RDWR | O_APPEND | O_CREAT, 0640);
        write(fileInt, strcat(list->title, "\n"), strlen(list->title));
        
        list = list->next;
    }

}


/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/

/*
This is the main input/output interface. It shows the options using printf
It then uses scanf to recieve a guaranteed integer input, which will then be returned. 
*/
int inputOption()
{
    int inputNumber;
    printf("\n1. Select file to Process\n");
    printf("2. Exit from the program\n\n");
    printf("Enter a choice from 1 to 2: ");
    scanf("%d", &inputNumber);
    return inputNumber;
}

/*
If the user inputs 1, then they will be led to this function
This function takes input from the user, and selectively detects which movies (structs) match with that year
Information is printed in the console
*/

int inputOption1(){
    int inputNumber;
    printf("\nWhich file you want to process?\n");
    printf("Enter 1 to pick the largest file\n");
    printf("Enter 2 to pick the smallest file\n");
    printf("Enter 3 to specify the name of a file\n\n");
    printf("Enter a choice from 1 to 3: ");
    scanf("%d", &inputNumber);
    return inputNumber;
}

void largestCSV()
{
    DIR *dir;
    struct dirent *ent;
    FILE *fileCSV;
    char *maxFile;
    long maxLength = 0;
    long fileLength = 0;
    if((dir = opendir("./")) != NULL) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            if(strncmp(ent->d_name, "movies_", strlen("movies_")) == 0)
            {
                fileCSV = fopen(ent->d_name, "rb");
                fseek(fileCSV, 0, SEEK_END);
                fileLength = ftell(fileCSV);

                if(fileLength > maxLength)
                {
                    maxLength = fileLength;
                    maxFile = ent->d_name;
                }

                fseek(fileCSV, 0 , SEEK_SET);
            }
        }
        processFile2(maxFile);
        closedir(dir);
    } 
    else 
    {
    /* could not open directory */
    perror ("");
    }
}

void smallestCSV()
{
    DIR *dir;
    struct dirent *ent;
    FILE *fileCSV;
    char *minFile;
    long minLength = 1000000000;
    long fileLength = 0;
    if((dir = opendir("./")) != NULL) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            if(strncmp(ent->d_name, "movies_", strlen("movies_")) == 0)
            {
                fileCSV = fopen(ent->d_name, "rb");
                fseek(fileCSV, 0, SEEK_END);
                fileLength = ftell(fileCSV);

                if(fileLength < minLength)
                {
                    minLength = fileLength;
                    minFile = ent->d_name;
                }

                fseek(fileCSV, 0 , SEEK_SET);
            }
        }
        processFile2(minFile);
        // printf("%s\n", minFile);
        
        closedir(dir);
    } 
    else 
    {
    /* could not open directory */
    perror ("");
    }
}

int fileCheckCSV()
{
    DIR *dir;
    struct dirent *ent;
    FILE *fileCSV;
    long fileLength = 0;
    char modPtrInput[102];
    char fileInput[100];
    char modFileInput[102];
    int isInDir = 0;
    printf("Enter the complete file name: ");
    scanf("%s", fileInput);
    if((dir = opendir("./")) != NULL) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            snprintf(modPtrInput, sizeof modPtrInput, "_%s_", ent->d_name);
            snprintf(modFileInput, sizeof modFileInput, "_%s_", fileInput);
            if(strncmp(modPtrInput, modFileInput, strlen(modFileInput)) == 0)
            {
                isInDir = 1;
            }          
        }
        if(isInDir == 1)
        {
            processFile2(fileInput);
        }
        else
        {
            printf("The file %s was not found. Try again\n\n", fileInput);
        }
        closedir(dir);
    } 
    else 
    {
    /* could not open directory */
    perror ("");
    }
    return isInDir;
}
int main(int argc, char *argv[])
{
    // if (argc < 2) //provide easy data input
    // {
    //     printf("You must provide the name of the file to process\n");
    //     printf("Example usage: ./movies movies_sample_1.csv\n");
    //     return EXIT_FAILURE;
    // }
    // struct movie *list = processFile(argv[1]); //creating linked list
    // printf("Processed file %s and parsed data for %i movies\n", argv[1], returnMovieCount(list)); //sending message to user to confirm success of linked list creation
    int inputNumber = 0; //recieving input value
    int inputNumber1 = 0;
    int returnVal3 = 0;
    while(1)
    {
        inputNumber = inputOption(); //input function, clean input
        if(inputNumber == 1) // user chooses option 1, call function that does option 1 task
        {
            while(1)
            {
                inputNumber1 = inputOption1();
                if(inputNumber1 == 1)
                {
                    largestCSV();
                    break;
                }
                else if(inputNumber1 == 2)
                {
                    smallestCSV();
                    break;
                }
                else if(inputNumber1 == 3)
                {
                    returnVal3 = fileCheckCSV();
                    if(returnVal3 == 1)
                    {
                        break;
                    } 
                }             
                else{
                    printf("You entered an incorrect choice. Try again.\n");
                }  
            }
        }
        else if(inputNumber == 2) // user chooses option 2, call function that does option 2 task
        {
            break;
        }
        else
        {
            printf("You entered an incorrect choice. Try again.\n"); // if integer is not within range
        }
    }

    return EXIT_SUCCESS;
}
