#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  // char buffer[256];
  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
    exit(0); 
  } 

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }
  // Get input message from user
  // printf("CLIENT: Enter text to send to the server, and then hit enter: ");
  // // Clear out the buffer array
  // memset(buffer, '\0', sizeof(buffer));
  // // Get input from the user, trunc to buffer - 1 chars, leaving \0
  // fgets(buffer, sizeof(buffer) - 1, stdin);

  /*
  THIS IS WHERE I REPLACE FGETS WITH FOPEN MESSAGE, FOPEN KEY 
  char array[] = fopen(something);
  CHECK FOR NONSTANDARD CRAP
  CHECK IF SIZE OF FILES ARE EQUAL IF NOT RETURN ERROR
  */


  FILE *message = fopen(argv[1], "rb");
  unsigned long strLen;
  char *buffer1 = 0;
  if(message)
  {
    fseek(message, 0L, SEEK_END);
    strLen = ftell(message);
    fseek(message, 0L, SEEK_SET);
    buffer1 = malloc(strLen+1);
    if(buffer1)
    {
      fread(buffer1, 1, strLen, message);
    }
    fclose(message);
  }
  buffer1[strLen] = '\0';

  FILE *key = fopen(argv[2], "rb");
  unsigned long keyLen;
  char *buffer2 = 0;
  if(key)
  {
    fseek(key, 0L, SEEK_END);
    keyLen = ftell(key);
    fseek(key, 0L, SEEK_SET);
    buffer2 = malloc(keyLen+1);
    if(buffer2)
    {
      fread(buffer2, 1, keyLen, key);
    }
    fclose(key);
  }
  buffer2[keyLen] = '\0';


  char *buffer = 0;
  printf("hi\n");
  buffer = malloc(strLen+keyLen+1);
  snprintf(buffer, strLen+keyLen+1, "%s%s", buffer1, buffer2);
  printf("buffer1: %s, buffer2: %s, buffer: %s \n", buffer1, buffer2, buffer);


  printf("CLIENT: Sending string length: %d\n", strLen);
  charsWritten = send(socketFD, &strLen, sizeof(int), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }

  printf("CLIENT: Sending key length: %d\n", keyLen);
  charsWritten = send(socketFD, &keyLen, sizeof(int), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }

  // Remove the trailing \n that fgets adds
  // buffer1[strcspn(buffer1, "\n")] = '\0'; 

  // Send message to server
  // Write to the server


  printf("CLIENT: Sending buffer: %s, totallen: %d\n", buffer, strLen+keyLen);
  charsWritten = send(socketFD, buffer, strLen+keyLen, 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(buffer)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  // Get return message from server
  // Clear out the buffer again for reuse
  memset(buffer, '\0', sizeof(buffer));
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, buffer, strLen+keyLen, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
  printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

  // Close the socket
  close(socketFD); 
  return 0;
}