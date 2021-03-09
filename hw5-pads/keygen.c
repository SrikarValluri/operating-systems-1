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

    srand(time(NULL));

    int keylength = atoi(argv[1]);

    char *key = malloc(keylength+1);
    char temp[keylength];
    int i = 0;
    int randomInt;

    while(i < keylength)
    {
        randomInt = rand() % 27;
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
    }
    key[keylength] = '\0';
    printf("%s\n", key);
    
}