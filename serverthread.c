
// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include<time.h>
#define PORT 7790
FILE *file;
char *buffer;
int fileLen;



char client_message[2000];
//char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * socketThread(void *arg)
{
  int new_socket = *((int *)arg);
  
 // recv(newSocket , client_message , 2000 , 0);
 
 char buf[30000] = {0};
        long valread = read( new_socket , buf, 30000);
        char request_type[8]; // GET or HEAD
        char request_path[1024]; // /info etc.
        char request_protocol[128]; // HTTP/1.1
        
        printf("Recieved: %s\n",buf );
        sscanf(buf, "%s %s %s", request_type, request_path, request_protocol);
      
       printf("request_type: %s\n", request_type);
       printf("request_protocol: %s\n", request_protocol);
     
       char toSearch = '/';
       int count = 0;
       int i=0;
       char file_name[strlen(request_path)];
     
       while(request_path[i] != '\0')
        {
        
        /*
         * If character is found in string then
         * increment count variable
         */
        if(request_path[i] == toSearch)
        {
            count++;
        }

        i++;
    }
       printf("count of backslash: %d\n",count);
       if (count >1){
        printf("Provided path: %s is bad. Files must be in thesame location as the server\n", request_path);
        return NULL;
       }
       printf("request_path: %s\n", request_path);
       memmove (file_name, request_path+1, strlen (request_path+1) + 1);
       int len = strlen(file_name);
       printf("File_name: %s length: %d\n", file_name,len);
       
       //Open file
       file = fopen(file_name, "r");
       if (!file)
        {
         printf("File not Found!\n");
         printf("Exit socketThread \n");
         close(new_socket);
         pthread_exit(NULL);
       //return NULL;
       }

      //Get file length
      fseek(file, 0, SEEK_END);
      fileLen=ftell(file);
      fseek(file, 0, SEEK_SET);

       //Allocate memory
      buffer=(char *)malloc(fileLen+1);
     if (!buffer)
     {
      printf("Memory error!\n");
      fclose(file);
     return NULL;
      }
    //read file contents into buffer
   fread(buffer, fileLen, 1, file);
   fclose(file);
 char header[102400];
 sprintf(header, 
"HTTP/1.1 200 OK\n"
"Server: Threaded Server\n"
"Content-Type: text/plain\n"
"Content-Length: %i\n"
"Accept-Ranges: bytes\n"
"Connection: close\n"
        "\n",fileLen);

  // Send message to the client socket 
  //pthread_mutex_lock(&lock);
  char *reply = (char*)malloc(strlen(header)+fileLen);
  strcpy(reply, header);
  strcat(reply, buffer);
/*  char *message = malloc(sizeof(client_message)+20);*/
/*  strcpy(message,"Hello Client : ");*/
/*  strcat(message,client_message);*/
/*  strcat(message,"\n");*/
/*  strcpy(buffer,message);*/
 // free(message);
  //pthread_mutex_unlock(&lock);
  //sleep(1);
  //send(newSocket,buffer,13,0);

  if(strcmp(request_type,"GET")==0){
  write(new_socket , reply , strlen(reply));
  }  
  else if(strcmp(request_type,"HEAD")==0){
  write(new_socket , header , strlen(header));
  }
   else {
        printf("HTTP - Method %s is not implemented\n",request_type); 
/*   char msg = "Not Implemented";*/
/*   int len = strlen(msg);*/
/*   char head[102400];*/
/* sprintf(head, */
/*"HTTP/1.1 404 NOT FOUND\n"*/
/*"Server: Threaded Server\n"*/
/*"Content-Type: text/plain\n"*/
/*"Content-Length: %i\n"*/
/*"Accept-Ranges: bytes\n"*/
/*"Connection: close\n"*/
/*        "\n",len);*/
/*        */
/* char *response = (char*)malloc(strlen(head)+len);*/
/*  strcpy(response, head);*/
/*  strcat(response, msg);*/
/*  write(new_socket , response , strlen(response));*/
/*  close(new_socket);*/
// exit(1);
   }
  //free(buffer);
  //free(reply);
  printf("Exit socketThread \n");
  close(new_socket);
  
  pthread_exit(NULL);
  sleep(1);
}

int main(int argc, char const *argv[])
{
     if (argc != 2) {
	   perror("usage: ./serverthread IP:PORT\n");
	   exit(1);
	}
     char delim[]=":";
     char *Desthost=strtok(argv[1],delim);
     char *Destport=strtok(NULL,delim);
     int port = atoi(Destport);
    
    int server_fd, new_socket; //long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Only this line has been changed. Everything is same.
   // char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = inet_addr(Desthost);
    address.sin_port = htons( port);
    
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        pthread_t tid;
        //int i = 0;
         if( pthread_create(&tid, NULL, socketThread, &new_socket) != 0 )
           printf("Failed to create thread\n");

      
            pthread_join(tid,NULL);
            pthread_cancel(tid);
            
       
    }
    return 0;
}
       
