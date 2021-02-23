#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>   
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <math.h> // must link with -lm


/*
A program with a pipeline of 3 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user and puts it in a buffer it shares with the next thread in the pipeline.
- Square root thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It computes the square root of this item. It puts the computed value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the third thread in the pipeline. It consumes items from the buffer it shares with the square root thread and prints the items.

*/

// Size of the buffers
#define SIZE 1000

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_ITEMS 49

// Buffer 1, shared resource between input thread and line-seperator thread
char buffer_1[NUM_ITEMS][SIZE];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the line-seperator thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between line-seperator thread and plus-sign thread
char buffer_2[NUM_ITEMS][SIZE];
// Number of items in the buffer
int count_2 = 0;
// Index where the line-seperator thread will put the next item
int prod_idx_2 = 0;
// Index where the output thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3, shared resource between plus-sign thread and output thread
char buffer_3[NUM_ITEMS][SIZE];
// Number of items in the buffer
int count_3 = 0;
// Index where the square-root thread will put the next item
int prod_idx_3 = 0;
// Index where the output thread will pick up the next item
int con_idx_3 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

/*
 Put an item in buff_1
*/
void put_buff_1(char *item)
{
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
  // Put the item in the buffer
  strcpy(buffer_1[prod_idx_1], item);
  // Increment the index where the next item will be put.
  prod_idx_1 = prod_idx_1 + 1;
  count_1++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
}

/*
Get input from the user.
This function doesn't perform any error checking.
*/
void get_user_input(char *user_input)
{
  fgets(user_input, 1000, stdin);
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer shared with the square_root thread.
*/
void *get_input()
{
    int i;
    char item[1000];
    for(i = 0; i < NUM_ITEMS; i++)
    {
      // Get the user input
      get_user_input(item);
      put_buff_1(item);
      if(strcmp(item, "STOP\n") == 0)
      {
        break;
      }
    }
    return NULL;
}

/*
Get the next item from buffer 1
*/
void get_buff_1(char* item)
{
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);
  strcpy(item, buffer_1[con_idx_1]);
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
}

/*
 Put an item in buff_2
*/
void put_buff_2(char* item)
{
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
  strcpy(buffer_2[prod_idx_2], item);
  // Increment the index where the next item will be put.
  prod_idx_2 = prod_idx_2 + 1;
  count_2++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
}

/*
 Function that the square root thread will run. 
 Consume an item from the buffer shared with the input thread.
 Compute the square root of the item.
 Produce an item in the buffer shared with the output thread.

*/

// This function is from my smallsh program
void replace_string(char *target, const char *key, const char *replacement) // this function is specifically for replacing every \n or ++ occurence with its respective replacement
// Target is the input from the user, key is the thing we want to replace, and replacement is what we will replace it with.
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


void *line_seperator(void *args)
{
    char item[1000];
    int i;    
    for(i = 0; i < NUM_ITEMS; i++)
    {
      get_buff_1(item);
      if(strcmp(item, "STOP\n") == 0)
      {
        put_buff_2(item);
        break;
      }
      replace_string(item, "\n", " ");
      put_buff_2(item);
    }
    return NULL;
}

/*
Get the next item from buffer 2
*/
void get_buff_2(char* item)
{
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);
  strcpy(item, buffer_2[con_idx_2]);
  // Increment the index from which the item will be picked up
  con_idx_2 = con_idx_2 + 1;
  count_2--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
}

/*
 Put an item in buff_2
*/
void put_buff_3(char *item)
{
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_3);
  // Put the item in the buffer
  strcpy(buffer_2[prod_idx_3], item);
  // Increment the index where the next item will be put.
  prod_idx_3 = prod_idx_3 + 1;
  count_3++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_3);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
}

void *plus_sign(void *args)
{
    char item[1000];  
    int i;
    for(i = 0; i < NUM_ITEMS; i++)
    {
      get_buff_2(item);
      if(strcmp(item, "STOP\n") == 0)
      {
        put_buff_3(item);
        break;
      }
      replace_string(item, "++", "^");
      put_buff_3(item);
    }
    return NULL;
}

/*
Get the next item from buffer 2
*/
void get_buff_3(char* item)
{
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_3);
  while (count_3 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_3, &mutex_3);
  strcpy(item, buffer_2[con_idx_3]);
  // Increment the index from which the item will be picked up
  con_idx_3 = con_idx_3 + 1;
  count_3--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
  // Return the item
}


/*
 Function that the output thread will run. 
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/
void *write_output()
{
    char item[1000];
    char formatted_item[100000];
    char buffer[100000];
    int new = 0;
    int i = 0;
    int j = 0;
    int k;
    int l;

    for(i = 0; i < NUM_ITEMS; i++)
    {
      get_buff_3(item);
      if(strcmp(item, "STOP\n") == 0)
      {
        break;
      }

      if(new == 0)
      {
        snprintf(formatted_item, sizeof formatted_item, "%s", item); //modifying original input
        new = 1;
      }
      else
      {
        strcpy(buffer, formatted_item);
        snprintf(formatted_item, sizeof formatted_item, "%s%s", buffer, item); //modifying original input
        // printf("formatted_item: %s\n", formatted_item);
      }
      // printf("len-j: %d\n", (strlen(formatted_item)-j));
      fflush(stdout);
      while((strlen(formatted_item) - j) >= 80)
      {
        k = j;
        j += 80;
        for(k; k < j; k++)
        {
          printf("%c", formatted_item[k]);
          fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
      }
    }

    return NULL;
}

int main()
{
    pthread_t input_t, line_seperator_t, plus_sign_t, output_t;
    // Create the threads
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&line_seperator_t, NULL, line_seperator, NULL);
    pthread_create(&plus_sign_t, NULL, plus_sign, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(line_seperator_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(output_t, NULL);
    return EXIT_SUCCESS;

    // char user_input[1000];
    // get_user_input(user_input);
    // replace_string(user_input, "\n", " ");
    // printf("user input: %stesting", user_input);

}