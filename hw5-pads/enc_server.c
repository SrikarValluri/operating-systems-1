#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>    // time()
#include <fcntl.h>
#include <signal.h>
#include <errno.h>


// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

/*
Citing my amazing self for creating the best creation of all time
*/
char numToChar(int randomInt)
{
  if(randomInt == 0)
  {
    return('A');
  }
  else if(randomInt == 1)
  {
    return('B');
  }
  else if(randomInt == 2)
  {
    return('C');
  }
  else if(randomInt == 3)
  {
    return('D');
  }
  else if(randomInt == 4)
  {
    return('E');
  }
  else if(randomInt == 5)
  {
    return('F');
  }
  else if(randomInt == 6)
  {
    return('G');
  }
  else if(randomInt == 7)
  {
    return('H');
  }
  else if(randomInt == 8)
  {
    return('I');
  }
  else if(randomInt == 9)
  {
    return('J');
  }
  else if(randomInt == 10)
  {
    return('K');
  }
  else if(randomInt == 11)
  {
    return('L');
  }
  else if(randomInt == 12)
  {
    return('M');
  }
  else if(randomInt == 13)
  {
    return('N');
  }
  else if(randomInt == 14)
  {
    return('O');
  }
  else if(randomInt == 15)
  {
    return('P');
  }
  else if(randomInt == 16)
  {
    return('Q');
  }
  else if(randomInt == 17)
  {
    return('R');
  }
  else if(randomInt == 18)
  {
    return('S');
  }
  else if(randomInt == 19)
  {
    return('T');
  }
  else if(randomInt == 20)
  {
    return('U');
  }
  else if(randomInt == 21)
  {
    return('V');
  }
  else if(randomInt == 22)
  {
    return('W');
  }
  else if(randomInt == 23)
  {
    return('X');
  }
  else if(randomInt == 24)
  {
    return('Y');
  }
  else if(randomInt == 25)
  {
    return('Z');
  }
  else
  {
    return(' ');
  }
}

int charToNum(int letter)
{
  if(letter == 'A')
  {
    return(0);
  }
  else if(letter == 'B')
  {
    return(1);
  }
  else if(letter == 'C')
  {
    return(2);
  }
  else if(letter == 'D')
  {
    return(3);
  }
  else if(letter == 'E')
  {
    return(4);
  }
  else if(letter == 'F')
  {
    return(5);
  }
  else if(letter == 'G')
  {
    return(6);
  }
  else if(letter == 'H')
  {
    return(7);
  }
  else if(letter == 'I')
  {
    return(8);
  }
  else if(letter == 'J')
  {
    return(9);
  }
  else if(letter == 'K')
  {
    return(10);
  }
  else if(letter == 'L')
  {
    return(11);
  }
  else if(letter == 'M')
  {
    return(12);
  }
  else if(letter == 'N')
  {
    return(13);
  }
  else if(letter == 'O')
  {
    return(14);
  }
  else if(letter == 'P')
  {
    return(15);
  }
  else if(letter == 'Q')
  {
    return(16);
  }
  else if(letter == 'R')
  {
    return(17);
  }
  else if(letter == 'S')
  {
    return(18);
  }
  else if(letter == 'T')
  {
    return(19);
  }
  else if(letter == 'U')
  {
    return(20);
  }
  else if(letter == 'V')
  {
    return(21);
  }
  else if(letter == 'W')
  {
    return(22);
  }
  else if(letter == 'X')
  {
    return(23);
  }
  else if(letter == 'Y')
  {
    return(24);
  }
  else if(letter == 'Z')
  {
    return(25);
  }
  else
  {
    return(26);
  }
}

int special_character(char *str){
   int i, flag = 0;
   for(i = 0; i < strlen(str); i++)
   {
      //checking each character of the string for special character.
      if(str[i] == '!' || str[i] == '@' || str[i] == '#' || str[i] == '$'
      || str[i] == '%' || str[i] == '^' || str[i] == '&' || str[i] == '*'
      || str[i] == '(' || str[i] == ')' || str[i] == '-' || str[i] == '{'
      || str[i] == '}' || str[i] == '[' || str[i] == ']' || str[i] == ':'
      || str[i] == ';' || str[i] == '"' || str[i] == '\'' || str[i] == '<'
      || str[i] == '>' || str[i] == '.' || str[i] == '/' || str[i] == '?'
      || str[i] == '~' || str[i] == '`' )
      {
        flag = 1;
        return flag;
      }
   }
   return flag;
}

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

void catch_SIGCHILD(int signal)
{
    int status = 0;
    while((waitpid(-1, &status, WNOHANG)) > 0){}
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead, strLen, keyLen, counter;
//   char *confirm_enc_client;
  char *buffer;
  char *encoded_buffer;
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);
  struct sigaction act; // https://www.ipa.go.jp/security/awareness/vendor/programmingv1/b07_04.html
  memset(&act, 0, sizeof(act));   
  act.sa_handler = catch_SIGCHILD;
  sigemptyset(&act.sa_mask); 
  act.sa_flags = SA_NOCLDSTOP | SA_RESTART;
  sigaction(SIGCHLD, &act, NULL);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  pid_t spawnpid;
  
  while(1){
    // spawnpid = fork();
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo);
    spawnpid = fork();

    if(spawnpid == 0)
    {
      if (connectionSocket < 0){
        error("ERROR on accept");
      }   
    //   char confirm_enc_client[1];
      
    //   charsRead = recv(connectionSocket, confirm_enc_client[0], 10, 0);


    //   if (charsRead < 0){
    //     error("ERROR reading from socket");
    //   }

    //   if(confirm_enc_client != 'S')
    //   {
    //       error("Incorrect Server.\n");
    //   }
      
      charsRead = send(connectionSocket, "E", 1, 0); 


      charsRead = recv(connectionSocket, &strLen, sizeof(int), 0);
      if (charsRead < 0){
        error("ERROR reading from socket");
      }
      
      charsRead = recv(connectionSocket, &keyLen, sizeof(int), 0);

      if (charsRead < 0){
        error("ERROR reading from socket");
      }

      buffer = malloc(strLen+keyLen+1);
      charsRead = recv(connectionSocket, buffer, strLen+keyLen+1, 0);

      if (charsRead < 0){
        error("ERROR reading from socket");
      }
      

      /*
      TAKE THE MESSAGE AND EITHER ENCRYPT OR DECRPYT BASED ON WHICH SERVER IT IS
      UPDATE CONNECTIONSOCKET with ENCRYPT VALUE

      */

      counter = 0;
      encoded_buffer = malloc(strLen+1);
      while(counter < strLen-1)
      {
        encoded_buffer[counter] = numToChar(mod((charToNum(buffer[counter]) + charToNum(buffer[counter+strLen])),27));
        counter++;
      }
      encoded_buffer[strLen-1] = '\0';

      // Send a Success message back to the client
      charsRead = send(connectionSocket, 
                      encoded_buffer, strlen(encoded_buffer), 0); 
      if (charsRead < 0){
        error("ERROR writing to socket");
      }
      // Close the connection socket for this client
      close(connectionSocket);
      exit(0); 
    }

    }

  // Close the listening socket
  close(listenSocket); 
  return 0;
}
