#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <sys/select.h>

#define SIZE 256




// From the terminal a signal is transmitted to this node S, which will then comunicate with P, 
// depending on the received information


// Initilisation of the three flags to handle the received signal
int Flag_1, Flag_2, Flag_Log;



// Initialisation of funcions
void Log_Printing();
void Log_Writing(char string[50]);



// Handling SIGUSR1 signal 
void Handler_SIGUSR1(int signo)

{
    Flag_1 = 1;

}

// Handling SIGUSR2 signal 
void Handler_SIGUSR2(int signo)

{

    Flag_2 = 1;

}

// Handling SIGINT signal 
void Handler_SIGINT(int signo)
{

    Flag_Log = 1;

}




// Main
int main(int argc, char *argv[])
{
    

    printf("Execution started: Process S\n");


    //Pipe from S to P
    int pipe_sp[2];
    pipe_sp[0] = atoi(argv[1]);
    pipe_sp[1] = atoi(argv[2]);



    //Print on Log file essential information
    Log_Writing((char*)"User Input Selection\n");
    printf("\nThree possible inputs:\n");
    printf("\n-SIGUSR1: Start Receiving Tokens \n-IGUSR2: Stop Receiving Tokens \n-SIGINT: Output Log File\n");
    printf("\nTo run the S process, open another terminat and digit \nkillall  (SIGUSR1 / SIGUSR2 / SIGINT) S\n");
    printf("\nEnter SIGUSR1, SIGUSR2 or SIGINT:\n");


    //Check inputs
    if (signal(SIGUSR1, Handler_SIGUSR1) == SIG_ERR)

    {
        printf("\nERROR: Impossible performing SIGUSR1\n");
    }



    if (signal(SIGUSR2, Handler_SIGUSR2) == SIG_ERR)

    {
        printf("\nERROR: Impossible performing SIGUSR2\n");
    }



    if (signal(SIGINT, Handler_SIGINT) == SIG_ERR)

    {
        printf("\nERROR: Impossible performing SIGINT\n");
        int selection = 0;
    }



    char Signal_Selection[SIZE];

    


    while(1)  //While receiving inputs
    {

        if (Flag_1 == 1 )

        {

            printf("Sending Tokens (SIGUSR1 received)\n"); 

            Log_Writing((char*)"Start Receiving and Sending Tokens\n");

            strcpy(Signal_Selection, "1"); // If the input received is 1, P reads and writes tokens

            close(pipe_sp[0]); // 1 is wrtitten on pipe_sp

            write(pipe_sp[1], &Signal_Selection, sizeof(Signal_Selection));

        }



        if (Flag_2 == 1 )

        {

            printf("Stop Sending Tokens (SIGUSR2 received)\n");

            Log_Writing((char*)"Stop Receiving Tokens\n");

            strcpy(Signal_Selection, "0"); // If the input received is 0, P stops to read and send tokens

            close(pipe_sp[0]); // 0 is wrtitten on pipe_sp

            write(pipe_sp[1], &Signal_Selection, sizeof(Signal_Selection));


        }



        if (Flag_Log == 1 )

        {

            printf("Printing Log: (received SIGINT)\n");

            Log_Writing((char*)"Print Log\n");

            Log_Printing(); // Writes on Log File

        }



        //Set flags to 0
        Flag_1 = 0;
        Flag_2 = 0;
        Flag_Log = 0;


        memset(Signal_Selection, 0, SIZE);


        //Wait for 2 seconds to have time to perform everything well
        sleep(2);

    }
}





// This functions allows to write on the Log File
void Log_Writing(char string[50])

{

    FILE * f;

    time_t t = time(NULL);

    struct tm * tm = localtime(&t);

    f = fopen("Log_File.log", "a+");



    if (f == NULL)

    {
        printf("Inability to open the file\n");
        exit(0);
    }

    fprintf(f, "\n-%s\nS: %s\n", asctime(tm), string );
    fclose(f);

}




// This functions allows to print the Log File's content
void Log_Printing()

{

    FILE * f;

    char c;

    f = fopen("Log_File.log", "r");



    if (f == NULL)

    {
        printf("Inability to open the file\n");
        exit(0);
    }


    c = fgetc(f);


    while (c != EOF)

    {
        printf ("%c", c);
        c = fgetc(f);
    }

    fclose(f);

}
