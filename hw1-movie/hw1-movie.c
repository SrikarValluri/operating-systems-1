// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for important movie information */
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

/*
Print data specifically for Option 2's formatting (year, rating, title)
*/
void printOption2(struct movie *aMovie)
{
    printf("%s %0.1f %s\n", aMovie->year,
           atof(aMovie->rating),
           aMovie->title);
}

/*
Print data specifically for Option 3's formatting (year, title)
*/

void printOption3(struct movie *aMovie)
{
    printf("%s %s\n", aMovie->year,
           aMovie->title);
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
    printf("1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of release of all movies in a specific language\n");
    printf("4. Exit from the program\n");
    printf("Enter a choice from 1 to 4: ");
    scanf("%d", &inputNumber);
    return inputNumber;
}

/*
If the user inputs 1, then they will be led to this function
This function takes input from the user, and selectively detects which movies (structs) match with that year
Information is printed in the console
*/

void optionOne(struct movie *list)
{
    int yearRequested;
    int numMovies = 0; //count for determining whether movies were even counted that year
    printf("Enter for the year for which you want to see movies: "); //user input message
    scanf("%d", &yearRequested); //user input
    while(list != NULL) //looping through the movie list
    {
        if(yearRequested == atoi(list->year)) //determining whether the year matched with the input request
        {
            printf("%s\n", list->title);
            numMovies++;
        }
        list = list->next;
    }
    if(numMovies == 0) //no movies matched with the year input condition
    {
        printf("No data about movies released in %d\n", yearRequested);
    }
    
}

/*
If the user inputs 2, then they will be led to this function
This function uses a stack system. As we iterate through the list, we find the best possible values for each year.
We then update an array (sometimes replacing) values in the array, so that for every year, there is only one movie struct within the array, with the highest rating.
Information is printed in the console
*/

void optionTwo(struct movie *list)
{
    struct movie *minMovies[121] = { NULL }; //array that contains the highest-rank structs
    int counter; //counter
    while(list != NULL) //iterating through list
    {
        counter = 0; //initialize counter
        while(1) //loop that runs forever, but stops with break statements
        {
            if(minMovies[counter] != NULL) //if there are structs already in the array
            {
                if(atoi(list->year) == atoi(minMovies[counter]->year)) //compare the year of the current list with the struct in the array
                {
                    if(strtod(list->rating, NULL) >= strtod(minMovies[counter]->rating, NULL)) //if they are the same year, compare the rating
                    {
                        minMovies[counter] = list; //replace the old struct with new struct if rating is higher, we want max rating for all years
                    }
                    break; //now that we've iterated and replaced, there's no point of continuing to iterate, so break
                }
                
            }
            else //if there is no structs in the array
            {
                minMovies[counter] = list; //add the first struct, it's arbitrary
                break; //break, don't loop through a list with nulls
            }
            counter++; //iterate through the array to check for new stored structs 
        }
        list = list->next; //iterate through the linked list
    }
    counter = 0; //reset counter
    while(minMovies[counter] != NULL) //iterate through the array
    {
        printOption2(minMovies[counter]); //print the neccessary information with the highest ratings, with part 2's required formatting
        counter++; //iterate through the array
    }

}

/*
If the user inputs 3, they require all the movies for a specific language. After an input is taken, we modify the input with a "tag" (aka an underscore before and after)
We then modify the struct movie-> languages to replace any characters other than alphabets to an underscore as well.
This way, we can determine whether a string subset will uniquely identify as a language within the string of all languages

*/

void optionThree(struct movie *list)
{
    char languageRequested[20]; //original user input
    char modLanguageRequested[22]; //modified user input
    char actualCharacter[100]; //modified struct movie->languages with _
    char *pointerChar; //pointer to iterate through characters
    int indexChar; //index to index the pointer
    int length; //number of characters
    int numMovies = 0; //check to determine whether there even were movies that were in a particular language
    printf("Enter the language for which you want to see movies: "); //language user input message
    scanf("%s", languageRequested); //language user input
    snprintf(modLanguageRequested, sizeof modLanguageRequested, "_%s_", languageRequested); //modifying original input

    while(list != NULL) //iterate through list
    {
        indexChar = 0; //initialize index
        pointerChar = list->languages; //pointer that points to languages
        length = strlen(pointerChar); //determining char length of languages
        while(indexChar < length) //iterating through the chars of the language string, parsing anything with [ ; or ], replacing it with _
        {
            if(pointerChar[indexChar] == '[' || pointerChar[indexChar] == ';' || pointerChar[indexChar] == ']')
            {
                actualCharacter[indexChar] = '_';
            }
            else
            {
                actualCharacter[indexChar] = pointerChar[indexChar]; //if it's part of the alphabet, then just directly copy to new array
            }
            indexChar++;
        }
        if(strstr(actualCharacter, modLanguageRequested) != NULL) //see if modified input is within the modified language string, if so, then same with non-modified counterparts
        {
            printOption3(list);
            numMovies++;
        }

        list = list->next;
    }
    if(numMovies == 0) //alternate case of no language match 
    {
        printf("No data about movies released in %s\n", languageRequested);
    }
}
int main(int argc, char *argv[])
{
    if (argc < 2) //provide easy data input
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_sample_1.csv\n");
        return EXIT_FAILURE;
    }
    struct movie *list = processFile(argv[1]); //creating linked list
    printf("Processed file %s and parsed data for %i movies\n", argv[1], returnMovieCount(list)); //sending message to user to confirm success of linked list creation
    int inputNumber = 0; //recieving input value
    while(1)
    {
        inputNumber = inputOption(); //input function, clean input
        if(inputNumber == 1) // user chooses option 1, call function that does option 1 task
        {
            optionOne(list);
        }
        else if(inputNumber == 2) // user chooses option 2, call function that does option 2 task
        {
            optionTwo(list);
        }
        else if(inputNumber == 3) // user chooses option 3, call function that does option 3 task
        {
            optionThree(list);
        }
        else if(inputNumber == 4) // user chooses option 4, call function that does option 4 task (break/exit)
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
