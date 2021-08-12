#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#define PORT 7799
#define BUFF_SIZE 30000
int main(int argc, char const *argv[]) {
  /* Avoid zombies. */
  if (argc != 2) {
	   perror("usage:./serverfork IP:PORT\n");
	   exit(1);
	}
  signal(SIGCHLD, SIG_IGN);

  char buffer[BUFF_SIZE + 1];      
  //long valread = read( new_socket , buf, 30000);
   char request_type[8]; // GET or HEAD
   char request_path[1024]; // /info etc.
   char request_protocol[128]; // HTTP/1.1
   FILE *file;
   char *buf;
   int fileLen;
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(struct sockaddr_in);

  /* listening socket */
  //int sock = create_server_socket(false);
  
  
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
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
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
    
    
   // while(1)
  
  
  
  /* connections + requests */
  printf("Accepting connection...\n\n");
  while (1) {
     //printf("\nWaiting for new connection . . .\n\n");
     int client = accept(server_fd, 
			(struct sockaddr*) &client_addr, 
			&len);
    if (client < 0){
    printf("Problem with accept call\n");
    //inform user of socket number - used in send and receive commands
    }
  

    /* fork child */
    pid_t pid = fork();
   if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }
    /* 0 to child, child's PID to parent */
    if (pid == 0) {  /** child **/
     //printf("Success on fork call\n");
      close(server_fd);   /* child's listening socket */
      // printf("Closed server_fd socket\n");
      /* request */
      bzero(buffer, sizeof(buffer));
      int bytes_read = recv(client, buffer, sizeof(buffer), 0); 
      if (bytes_read < 0){
       printf("Problem with recv call\n");
      }
      /* response */
      
      //printf("Success on recv call\n");
      printf("Recieved: %s\n",buffer );
      sscanf(buffer, "%s %s %s", request_type, request_path, request_protocol);
      printf("Buffer: %s\n", buffer);
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
         perror("File not Found!\n");
       return NULL;
       }

      //Get file length
      fseek(file, 0, SEEK_END);
      fileLen=ftell(file);
      fseek(file, 0, SEEK_SET);

       //Allocate memory
       bzero(buffer, sizeof(buffer));
      buf=(char *)malloc(fileLen+1);
     if (!buf)
     {
      perror("Memory error!\n");
      fclose(file);
     return NULL;
      }
    //read file contents into buffer
   fread(buf, fileLen, 1, file);
   fclose(file);

   char header[102400];

 sprintf(header, 
"HTTP/1.1 200 OK\n"
"Server: Forked Server\n"
"Content-Type: text/plain\n"
"Content-Length: %i\n"
"Accept-Ranges: bytes\n"
"Connection: close\n"
        "\n",fileLen);

  // Send message to the client socket 
  //pthread_mutex_lock(&lock);
  char *reply = (char*)malloc(strlen(header)+fileLen);
  strcpy(reply, header);
  strcat(reply, buf);
/*  char *message = malloc(sizeof(client_message)+20);*/
/*  strcpy(message,"Hello Client : ");*/
/*  strcat(message,client_message);*/
/*  strcat(message,"\n");*/
/*  strcpy(buffer,message);*/
 // free(message);
  //pthread_mutex_unlock(&lock);
  //sleep(1);
  //send(newSocket,buffer,13,0);
 
 
 int bytes_written;
 if(strcmp(request_type,"GET")==0){
  bytes_written = write(client , reply , strlen(reply));
  }  
  else if(strcmp(request_type,"HEAD")==0){
  bytes_written = write(client , header , strlen(header));
  }
   else {
        printf("HTTP - Method %s is not implemented\n",request_type);
        
   }      
 
 
      
      
      
      
      
     // char response[BUFF_SIZE * 2]; 
     // bzero(response, sizeof(response));
     // generate_echo_response(buffer, response);
     
      //int bytes_written = send(client, response, strlen(response), 0); 
      if (bytes_written < 0) {
      perror("Problem with send call");
      }

      close(client); 
      exit(0);       /* terminate */
    } 
    else             /** parent **/
   // perror("Error In fork call");
        
      close(client); /* parent's read/write socket. */
      //exit(1);
  } 

  return 0; 
}

