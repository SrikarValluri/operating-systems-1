#include <stdio.h>

struct movie{
    char *title;
    char *year;
    char *languages;
    char *rating;
    struct movie *next;
};


struct movie *createMovie(char *currLine){
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
   token = strtok_r(NULL, "\n", &saveptr);
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

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

    // Read the file line by line
    while((getline(&currLine, &len, movieFile)) != -1)
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
            head -> next = tail;
        }
        else
        {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail -> next = newNode;
            tail = newNode;
        }
    }
    free(currLine);
    fclose(movieFile);
    return head;
}

/*
* Print data for the given movie
*/
void printMovie(struct movie* movie){
  printf("%s, %s %s, %s\n", movie->title,
               movie->year,
               movie->languages,
               movie->rating);
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

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/

int main(int argc, char *argv[])
{

   //  struct movie *list = processFile("./movies_sample_1.csv");
   //  printMovieList(list);
   //  return 0;

   FILE *movieFile = fopen("./movies_sample_1.csv", "r");
   char *currLine = NULL;
   size_t len = 0;   
   struct movie *temp;

   while((getline(&currLine, &len, movieFile)) != -1){
      temp = createMovie(currLine);
      printMovie(temp);
   }




}



// int main () {
//    FILE *fp;
//    int c;
  
//    fp = fopen("movies_sample_1.csv","r");
//    while(1) {
//       c = fgetc(fp);
//       if(feof(fp)) { 
//          break;
//       }
//       printf("%c", c);
//    }
//    fclose(fp);
   
//    return(0);
// }