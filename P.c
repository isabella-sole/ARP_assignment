#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <time.h>
#include <stdbool.h>
#include <arpa/inet.h>



#define SIZE 256



double New_Token(double received_token, double time_delay, double RF ); // Initialize function


void error(char *my_token); // Initialize function
 

void Write_Log(char string[50]); // Initialize function



struct my_token // Initialize structure

{

    double token;

    double timestamp;

};



struct my_token_to_L // Initialize structure
{
    double token;
    double timestamp;
    bool received;
};


// Main
int main(int argc, char *argv[])
{
    

    printf("Starting Execution: Process P\n");


    fd_set file_descriptor_select;

    
    // Create Variables
    int  signaL_Received_int, my_select, max_fd, res, res1, res2, res3, res4;

    int first_pipe = atoi(argv[5]); 

    int second_pipe = atoi(argv[7]); 


    double TokenReceived, TokenToSend,TimestampReceived, TimestampToSend, DT, ReferenceFrequency, WaitingTime;


    bool cont_while, unconnected;


    char signaL_Received[SIZE];


    // Create structures
    struct my_token ReceivedMessage;

    struct my_token MessageToSend;

    struct my_token_to_L L_Sent, L_Received;


    
    // Initialization
    ReferenceFrequency = atof(argv[1]); // Set a desired Reference Frequency

    WaitingTime = atof(argv[2])/1000000; // Set a Waiting Time

    signaL_Received_int= 1;

    TokenReceived = 0.0;

    TokenToSend = 0.0;

    TimestampReceived = time(NULL);


    printf("RF=%f   WT=%f\n", ReferenceFrequency, WaitingTime);

    
    int sockfd, portno, n; // Variables creation

    struct sockaddr_in serv_addr; // Struct creation

    portno = atoi(argv[4]); // Port Number

    sockfd = socket(AF_INET, SOCK_STREAM, 0);



    if (sockfd < 0)

    {

        error((char*)"ERROR while opening socket\n");

    }


    //Socket variables initialization
    bzero((char *) &serv_addr, sizeof(serv_addr));


    // In this new version of the assignment, the server should be on the same machine
    if(inet_pton(AF_INET, argv[3], &serv_addr.sin_addr)<=0)

    {

        error((char*)"NO ADRESSES FOUND: insert a valid address\n");

    }



    serv_addr.sin_family = AF_INET;

    serv_addr.sin_port = htons(portno);


    // Server Conncection
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)

    {

        error((char*)"ERROR during connection\n");

    }



    // While connection is estabilished
    do

    {
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)

        {

        }


        else

        {
            unconnected = false;

            printf("Connection estabilished\n");

        }


        if (!unconnected)

        {

            cont_while = false;

        }

    }



    while (cont_while);


    while(1) // Read from SELECT

    {

        FD_ZERO(&file_descriptor_select); //Initialize to 0

        FD_SET(first_pipe, &file_descriptor_select); 

        FD_SET(second_pipe, &file_descriptor_select); 

        max_fd = first_pipe >second_pipe ? first_pipe : second_pipe;

       
        my_select = select(max_fd+1, &file_descriptor_select, NULL, NULL, NULL); // Read from two pipes


        if (my_select == -1)

        {

            error((char*)"Error with select()\n");

        }


        // Three cases
        if(FD_ISSET(first_pipe, &file_descriptor_select) && (FD_ISSET(second_pipe, &file_descriptor_select) &&  	 (signaL_Received_int == 1)))

        {
           
            // Read from S
            // Read from first_pipe
            res1 = read(first_pipe, &signaL_Received, sizeof(signaL_Received));

            //Convert from char to int
            sscanf(signaL_Received, "%d", &signaL_Received_int);



            // Read from G
            // Read from second_pipe
            res2 = read(second_pipe, &ReceivedMessage, sizeof(ReceivedMessage));

            TokenReceived = ReceivedMessage.token;

            TimestampReceived = ReceivedMessage.timestamp;

        }


        else if (FD_ISSET(first_pipe, &file_descriptor_select))


        {
            
            // Read from S
            // Read from first_pipe
            res = read(first_pipe, &signaL_Received, sizeof(signaL_Received));

            //Convert from char to int
            sscanf(signaL_Received, "%d", &signaL_Received_int);

        }


        else if (FD_ISSET(second_pipe, &file_descriptor_select) && (signaL_Received_int == 1))

        {

  
            // Read from G
            // We can read from second_pipe
            res = read(second_pipe, &ReceivedMessage, sizeof(ReceivedMessage));


            //Convert from char to double
            TokenReceived = ReceivedMessage.token;

            TimestampReceived = ReceivedMessage.timestamp;

      
  }


        // Receive-Send Tokens
        if(signaL_Received_int == 1) 

        {

            //Send the received token to L for Log file
            L_Received.token = TokenReceived;

            L_Received.timestamp = TimestampReceived;

            L_Received.received = true;

            write(atoi(argv[6]), &L_Received, sizeof(L_Received));


            //Compute the sending token
            TimestampToSend = time(NULL); //timestamp

            DT =  WaitingTime + TimestampToSend - TimestampReceived; //Difference time

            printf("\nTime Received: %f.\nTime Sent: %f\nDT: %f\n", TimestampReceived, TimestampToSend, DT);

            //Creating the message to send to G using the socket
            TokenToSend = New_Token(TokenReceived, DT, ReferenceFrequency); //Compute the new token

            MessageToSend.token = TokenToSend;

            MessageToSend.timestamp = TimestampToSend;

            printf("\nReceiving/Sending tokens.\nP: Token Received: %f.\nP: Token Sent: %f\n", TokenReceived, TokenToSend);

            
            n = write(sockfd, &MessageToSend, sizeof(MessageToSend)); //Write message on socket
            if (n < 0) // Check

            {

                error((char*)"\nERROR while writing on socket.\n");

            }


            //Send the sent token to L for Log file
            L_Sent.token = TokenToSend;

            L_Sent.timestamp = WaitingTime + TimestampToSend; 

            L_Sent.received = false;

            write(atoi(argv[6]), &L_Sent, sizeof(L_Sent));


        }

        else if(signaL_Received_int == 0)    //If signal received from S is =0, start receiving and sending tokens

        {

            Write_Log((char*)"Stop receiving and sending tokens.\n");

        }


        else

        {
            printf("\nERROR.\n");
        }

        //Wait for 2 seconds 
        sleep(2);

    } // Finish while loop

    return 0;

} // Finish main


//Functions
double New_Token(double received_token, double time_delay, double RF )
{
    double new_token;

    new_token = received_token + time_delay*(1.0 - (pow(received_token,2.0))/2)*2*M_PI*RF;

    return new_token;
}




void error(char *my_token)
{
    perror(my_token);

    exit(0);
}




void Write_Log(char string[50])
{
    FILE * f;

    time_t t = time(NULL);

    struct tm * tm = localtime(&t);

    f = fopen("Log_File.log", "a+");

    if (f == NULL)

    {
        printf("Impossible to open the file\n");

        exit(0);

    }

    fprintf(f, "\n-%s P: %s\n", asctime(tm), string);

    fclose(f);

}
