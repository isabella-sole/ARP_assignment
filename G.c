#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>



#define SIZE 256



struct my_token   // Define struct
{

    double token;
    double timestamp;

};



void error(char *my_token) // Define function
{

printf("%s\n",my_token);
exit(0);

}


// Main
int main(int argc, char *argv[])

{
    

    char token_buffer[SIZE];

    double token_to_send, token_received;

    token_to_send = 0;

    struct my_token message_to_send, message_received;

    printf("Starting Execution of G Process\n");


    int sockfd, portno, newsockfd, clilen, n;

    // Internet adresses
    struct sockaddr_in serv_addr, cli_addr; 

    struct hostent *server;

    portno = atoi(argv[1]);  

    printf("G: Port Number = %d\n", portno);

    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Socket Creation


    if (sockfd < 0)

    {
        error((char*)"ERROR while opening socket\n");
   
    }



    //Initialize socket
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    serv_addr.sin_addr.s_addr = INADDR_ANY;

    serv_addr.sin_port = htons(portno);


    // Link socket to address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    
    {
    
         error((char*)"ERROR while binding\n");
    
    }


    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  (socklen_t*)&clilen);
    
    if (newsockfd < 0)
    
    {
    
         error((char*)"ERROR while accepting the client\n");
    
    }




    while(1)
    {
        
        n = read(newsockfd, &message_received, sizeof(message_received)); // Read the socket

        if (n < 0)

            error((char*)"ERROR while reading from socket\n");

        printf("G: Token:%f, Time Received: %f\n", message_received.token, message_received.timestamp);

       
        token_to_send = message_received.token; // Create token

        close(atoi(argv[2]));

        
        message_to_send.token = token_to_send; // Create message
        message_to_send.timestamp = time(NULL);

        
        write(atoi(argv[3]), &message_to_send, sizeof(message_to_send));


    }

}
