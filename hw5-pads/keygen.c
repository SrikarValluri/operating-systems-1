#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()
#include <fcntl.h>
#include <signal.h>
#include <errno.h>


int main(int argc, char *argv[])
{
    if (argc < 2){ 
    fprintf(stderr,"USAGE: %s <keylength> \n", argv[0]); 
    exit(0); 
    } 

    srand(time(NULL)); // setting random time seed

    int keylength = atoi(argv[1]); // turning the second terminal input into an integer, using as parameter

    char *key = malloc(keylength+1); // allocation of memory based on the needs of the key
    char temp[keylength]; // creating a temporary array
    int i = 0; // initialization of loop counter
    int randomInt; // temporary random integer reciever

    while(i < keylength) // while the amount of random characters have not yet been reached
    {
        randomInt = rand() % 27; // generate a random integer

        // Make sure each integer is set with its own corresponding letter

        if(randomInt == 0) 
        {
            key[i] = 'A';
        }
        else if(randomInt == 1)
        {
            key[i] = 'B';
        }
        else if(randomInt == 2)
        {
            key[i] = 'C';
        }
        else if(randomInt == 3)
        {
            key[i] = 'D';
        }
        else if(randomInt == 4)
        {
            key[i] = 'E';
        }
        else if(randomInt == 5)
        {
            key[i] = 'F';
        }
        else if(randomInt == 6)
        {
            key[i] = 'G';
        }
        else if(randomInt == 7)
        {
            key[i] = 'H';
        }
        else if(randomInt == 8)
        {
            key[i] = 'I';
        }
        else if(randomInt == 9)
        {
            key[i] = 'J';
        }
        else if(randomInt == 10)
        {
            key[i] = 'K';
        }
        else if(randomInt == 11)
        {
            key[i] = 'L';
        }
        else if(randomInt == 12)
        {
            key[i] = 'M';
        }
        else if(randomInt == 13)
        {
            key[i] = 'N';
        }
        else if(randomInt == 14)
        {
            key[i] = 'O';
        }
        else if(randomInt == 15)
        {
            key[i] = 'P';
        }
        else if(randomInt == 16)
        {
            key[i] = 'Q';
        }
        else if(randomInt == 17)
        {
            key[i] = 'R';
        }
        else if(randomInt == 18)
        {
            key[i] = 'S';
        }
        else if(randomInt == 19)
        {
            key[i] = 'T';
        }
        else if(randomInt == 20)
        {
            key[i] = 'U';
        }
        else if(randomInt == 21)
        {
            key[i] = 'V';
        }
        else if(randomInt == 22)
        {
            key[i] = 'W';
        }
        else if(randomInt == 23)
        {
            key[i] = 'X';
        }
        else if(randomInt == 24)
        {
            key[i] = 'Y';
        }
        else if(randomInt == 25)
        {
            key[i] = 'Z';
        }
        else
        {
            key[i] = ' ';
        }
        i++;

        // this creates a randomized key
    }
    key[keylength] = '\0'; // format the end of the string
    printf("%s\n", key); // send key to stdout 
    
}