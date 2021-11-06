#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <stdbool.h>



#define SIZE 256



void WritingLogReceived(double ReceivedToken, double ReceivedTime, int Iteration); // Initialize function


void WritingLogSent(double SentToken, double TimeSent, int Iteration); // Initialize function


void error(char *my_token); // Initialize function


struct my_token_to_L // Initialize struct
{

    double token;
    double timestamp;
    bool received;

};



// Main
int main(int argc, char *argv[])
{


    printf("Starting Execution of L Process\n");


    struct my_token_to_L my_token_received;

    int Iteration; // To number iterations performed

    Iteration = 0; // Set to zero the iterations 
    
    close(atoi(argv[2])); // To close writing of the pipe

    

    while(1)  // Performed on Log
    {

        read(atoi(argv[1]), &my_token_received, sizeof(my_token_received));  // Read the data from P


        // Flag check
        if(my_token_received.received == true)
	
	{

        WritingLogReceived(my_token_received.token, my_token_received.timestamp, Iteration);
        }


	else 

	{

        WritingLogSent(my_token_received.token, my_token_received.timestamp, Iteration);

        Iteration ++;

        }
    }

    return(0);
}



void error(char *my_token)
{
    perror(my_token);
    exit(0);
}




void WritingLogReceived(double ReceivedToken, double ReceivedTime, int Iteration)
{

    FILE * f;
    f = fopen("Log_File.log", "a");

    if (f == NULL)

    {
        printf("Cannot open file. \n");
        exit(0);
    }


    fprintf(f, "\n[%d]\n-%f: Token Received: <%f>", Iteration, ReceivedTime, ReceivedToken );
    
    fclose(f);

}




void WritingLogSent(double SentToken, double TimeSent, int Iteration)
{

    FILE * f;
    f = fopen("Log_File.log", "a");

    if (f == NULL)

    {
        printf("Cannot open file. \n");
        exit(0);
    }


    fprintf(f, "\n[%d]\n-%f: Token Sent: <%f>",Iteration, TimeSent, SentToken );
   
    fclose(f);
}

