#include<stdio.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include <netinet/in.h> 
#include<unistd.h>


#define DEFAULT_STRLEN 100
#define SERVER_PORT  23456

int create_server_sock(struct sockaddr_in *address);
void handle_client(int clientfd);
void reverse_input(char *word, int begin, int end);

int main(void)
{
    int sockfd;
    
    struct sockaddr_in address; 
    
          
    /* Create and bind socket using the function create_server_sock. Print an error message if unsucessful*/
    sockfd = create_server_sock(&address);
    if (sockfd == -1) {
        printf("Failed to create socket\n");
        return -1;
    } else if (sockfd == -2) {
        printf("Error binding socket\n");
        return -2;
    }
    
    /* Listen for incoming connections. Print an error message if unsucessful */
    if(listen(sockfd, SOMAXCONN) < 0) {
        printf("Error: Failed to listen for connections\n"); 
        return 0;
    }
               
        /* Accept */
        while(1){
        int addrlen = sizeof(address); 
        int clientfd = accept(sockfd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen);
    
        /* Handle client */
        handle_client(clientfd);
        break;
        }
    return 0;
}

int create_server_sock(struct sockaddr_in *address)
{
    int fd;
    
   // Create socket file descriptor. Return -1 if error
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        return -1;
    }

    // Define Server address
    address -> sin_family = AF_INET;
    address -> sin_addr.s_addr = INADDR_ANY; // any address
    address -> sin_port = htons(SERVER_PORT); // port

       
    // Bind socket to server address. Return -2 if error
    if (bind(fd, (struct sockaddr *)address, sizeof(struct sockaddr_in)) < 0) {
        return -2;
    }
    
    //return socket file descriptor
    return fd;
}


void handle_client(int clientfd)
{
    char outmsg[DEFAULT_STRLEN];
    char inmsg[DEFAULT_STRLEN];
    

    if(clientfd < 0) {
        printf("Error: Failed to accept client connection\n"); 
        return;
    }
     /* Read the msg from the client*/
    ssize_t r = recv(clientfd, inmsg, DEFAULT_STRLEN, 0);
    if(r <= 0) {
        printf("Error receiving message");
        close(clientfd);
        return;
    }
     /* Reverse the msg using function reverse_input*/
     strcpy(outmsg, inmsg);
     reverse_input(outmsg, 0, r);
     
    
     /* Send the reversed string back to client*/
    ssize_t s = send(clientfd, outmsg, r, 0);
    if(s < 0) {
        printf("Error sending message");
        close(clientfd);
        return;
    }
}

void reverse_input(char *word, int begin, int end){
char temp;
end--;
    while (begin < end) {
        temp = word[begin];
        word[begin] = word[end];
        word[end] = temp;
        begin++;
        end--;
    }

}
