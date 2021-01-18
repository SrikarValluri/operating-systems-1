// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for movie information */
struct movie
{
    char *title;
    char *year;
    char *languages;
    char *rating;
    struct movie *next;
};

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

void printOption2(struct movie *aMovie)
{
    printf("%s %s %s\n", aMovie->year,
           aMovie->rating,
           aMovie->title);
}

void printOption3(struct movie *aMovie)
{
    printf("%s %s\n", aMovie->year,
           aMovie->title);
}


/*
* Print the linked list of movies
*/
void printMovieList(struct movie *list)
{
    while (list != NULL)
    {
        printMovie(list);
        list = list->next;
    }
}

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

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/

int inputOption()
{
    int inputNumber;
    printf("1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of release of all movies in a specific language\n");
    printf("4. Exit from the program\n");
    printf("Enter a choice from 1 to 4: ");
    scanf("%d", &inputNumber);
    return inputNumber;
}

void optionOne(struct movie *list)
{
    int yearRequested;
    int numMovies = 0;
    printf("Enter for the year for which you want to see movies: ");
    scanf("%d", &yearRequested);
    while(list != NULL)
    {
        if(yearRequested == atoi(list->year))
        {
            printf("%s\n", list->title);
            numMovies++;
        }
        list = list->next;
    }
    if(numMovies == 0)
    {
        printf("No data about movies released in %d\n", yearRequested);
    }
    
}

void optionTwo(struct movie *list)
{
    struct movie *minMovies[121] = { NULL };
    int counter;
    while(list != NULL)
    {
        counter = 0;
        while(1)
        {
            if(minMovies[counter] != NULL)
            {
                if(atoi(list->year) == atoi(minMovies[counter]->year))
                {
                    if(strtod(list->rating, NULL) >= strtod(minMovies[counter]->rating, NULL))
                    {
                        minMovies[counter] = list;
                    }
                    break;
                }
                
            }
            else
            {
                minMovies[counter] = list;
                break;
            }
            counter++;
        }
        list = list->next;
    }
    counter = 0;
    while(minMovies[counter] != NULL)
    {
        printOption2(minMovies[counter]);
        counter++;
    }

}

void optionThree(struct movie *list)
{
    char languageRequested[20];
    int numMovies = 0;
    printf("Enter the language for which you want to see movies: ");
    scanf("%s", languageRequested);
    while(list != NULL)
    {
        if(strstr(list->languages, languageRequested) != NULL)
        {
            printOption3(list);
            numMovies++;
        }
        list = list->next;
    }
    if(numMovies == 0)
    {
        printf("No data about movies released in %s\n", languageRequested);
    }
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movie_info1.txt\n");
        return EXIT_FAILURE;
    }
    struct movie *list = processFile(argv[1]);
    printf("Processed file %s and parsed data for %i movies\n", argv[1], returnMovieCount(list));
    // printMovieList(list);
    int inputNumber = 0;
    while(1)
    {
        inputNumber = inputOption();
        if(inputNumber == 1)
        {
            optionOne(list);
        }
        else if(inputNumber == 2)
        {
            optionTwo(list);
        }
        else if(inputNumber == 3)
        {
            optionThree(list);
        }
        else if(inputNumber == 4)
        {
            break;
        }
        else
        {
            printf("You entered an incorrect choice. Try again.\n");
        }
    }

    return EXIT_SUCCESS;
}
