#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <signal.h>

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

/*
This function was inspired by this link: https://www.tutorialspoint.com/program-to-check-if-a-string-contains-any-special-character-in-c
All it does is iterates through the entire string, looking specifically for any special characters that it may come across, and return whether there exists any or not.
*/

int special_character(char *str){
   int i, flag = 0;
   for(i = 0; i < strlen(str); i++) // looping through string
   {
      // all the possible invalid characters
      if(str[i] == '!' || str[i] == '@' || str[i] == '#' || str[i] == '$'
      || str[i] == '%' || str[i] == '^' || str[i] == '&' || str[i] == '*'
      || str[i] == '(' || str[i] == ')' || str[i] == '-' || str[i] == '{'
      || str[i] == '}' || str[i] == '[' || str[i] == ']' || str[i] == ':'
      || str[i] == ';' || str[i] == '"' || str[i] == '\'' || str[i] == '<'
      || str[i] == '>' || str[i] == '.' || str[i] == '/' || str[i] == '?'
      || str[i] == '~' || str[i] == '`' || str[i] == '|')
      {
        flag = 1;
        return flag; // if exists return 1
      }
   }
   return flag; // otherwise return 0
}

int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead; // important information for sending information between client and server
  struct sockaddr_in serverAddress;
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
  /*
  The next parts open the message, reads through the message, and stores important information
  such as the length of the message and the message itself (buffer1). It uses the functions fseek,
  ftell, and fread. 
  */

  FILE *message = fopen(argv[1], "rb"); 
  unsigned long strLen; 
  char *buffer1 = 0; 
  if(message)
  {
    fseek(message, 0L, SEEK_END); // read through the end of the message
    strLen = ftell(message); // count the number of characters
    fseek(message, 0L, SEEK_SET); // set the pointer back to the initial point
    buffer1 = malloc(strLen+1); 
    if(buffer1)
    {
      fread(buffer1, 1, strLen, message);
    }
    fclose(message);
  }
  buffer1[strLen] = '\0'; // formatting end of string
  buffer1[strcspn(buffer1, "\n")] = '\0'; // removing newline

 /*
  The next parts open the KEY, reads through the KEY, and stores important information
  such as the length of the KEY and the KEY itself (buffer2). It uses the functions fseek,
  ftell, and fread, similar to the first one. 
*/

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
  buffer2[keyLen] = '\0'; // formatting strings once again
  buffer2[strcspn(buffer2, "\n")] = '\0'; // removing endline

  /*
  This step exits the program if the key has invalid characters, or if it has less characters than the message itself.
  Without conditions being met, the program cannot run successfully. 

  */

  if(strLen > keyLen)
  {
      fprintf(stderr, "Invalid Key.\n");
      exit(1);
  }

  if(special_character(buffer1) || special_character(buffer2))
  {
      fprintf(stderr, "Invalid Character.\n");
      exit(1);
  }

  /*
  This step will concatenate the message and the key so that it provides for easy transport between the client and the server.
  */

  char *buffer = 0;
  buffer = malloc(strLen+keyLen+1);
  snprintf(buffer, strLen+keyLen+1, "%s%s", buffer1, buffer2); // concatenation

  // This step checks whether the enc_client is going to the enc_server, because it doesn't belong to the dec_server
  // Making sure to error if necessary


  char confirm_enc_client[2];
  memset(confirm_enc_client, '\0', 2);

  charsWritten = recv(socketFD, confirm_enc_client, 1, 0);

  if(confirm_enc_client[0] != 'E')
  {
    fprintf(stderr, "Incorrect Server.\n");
    exit(2);
  }

  /*
  Sending information about the length of the message itself, as well as the length of the key. 
  This is using the send function, something that is crucial to communication between the client and server.
  */

  charsWritten = send(socketFD, &strLen, sizeof(int), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }


  charsWritten = send(socketFD, &keyLen, sizeof(int), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }

  /*
  This code is sending the buffer itself, with all the information about the message and the key
  */

  charsWritten = send(socketFD, buffer, strLen+keyLen, 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(buffer)){ // errors if required
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  // Get return message from server
  // Clear out the buffer again for reuse

  /*
  Now it's time to recieve the encrypted information back from the server. This encryption is the result of the 
  message and key that was sent to the server. This encryption is then formatted and then printed in stdout. 
  */

  memset(buffer, '\0', sizeof(buffer));
  char *encoded_buffer;
  encoded_buffer = malloc(strLen+1);
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, encoded_buffer, strLen, 0);
  encoded_buffer[strLen-1] = '\0';
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
//   printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
  printf("%s\n", encoded_buffer);



  // Close the socket
  close(socketFD); 
  return 0;
}