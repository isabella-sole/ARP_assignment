#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>





// The Launcher.c file is used to initialize all processe and creates pipes





int main()
{


    
    FILE *config_file; // Read data from configuration file
    config_file = fopen("config.txt", "r");



    // Initiliaze variables for IP, port number, reference frequnecy, waiting time
    char ip[20], portnumber[20], referencefrequency[20], waitingtime[20];  



    // Scan from configuration file, set as reference frequency, waiting time, ip, port number
    fscanf(config_file, "%s", referencefrequency);
    fscanf(config_file, "%s", waitingtime);
    fscanf(config_file, "%s", ip);
    fscanf(config_file, "%s", portnumber);

   

    // Initialize processes
    pid_t P_process; // Initialize P process
    pid_t L_process; // Initialize L process
    pid_t S_process; // Initialize S process
    pid_t G_process; // Initialize G process



    //These allow, for each process, to return varibles for child
    int status_P_child, status_L_child, status_S_child, status_G_child; 

    

    
    int pipe_sp[2]; //From process S to P
    int pipe_gp[2]; //From process G to P
    int pipe_pl[2]; //From process P to L


    
    if (pipe(pipe_sp) < 0)  // Security check for a correct creation of pipe SP
    
    {
        perror("ERROR PIPE_SP. Creation failed.\n");
        return -1;
    }



    if (pipe(pipe_gp) < 0)  // Security check for a correct creation of pipe GP

    {
        perror("ERROR PIPE_GP. Creation failed.\n");
        return -1;
    }



    if (pipe(pipe_pl) < 0)  // Security check for a correct creation of pipe PL

    {
        perror("ERROR PIPE_PL. Creation failed.\n");
        return -1;
    }




    // Creates arrays with data for the exec
    char *argumentP[12];
    char *argumentG[5];
    char *argumentS[4];
    char *argumentL[4];



    // Buffers where to read and write
    char pipe_sp_r[5]; // Read pipe_sp
    char pipe_sp_w[5]; // Write pipe_sp
    char pipe_gp_r[5]; // Read pipe_gp
    char pipe_gp_w[5]; // Write pipe_gp
    char pipe_pl_r[5]; // Read pipe_pl
    char pipe_pl_w[5]; // Write pipe_pl



    // Conversion from int to char
    sprintf(pipe_sp_r, "%d", pipe_sp[0]); 
    sprintf(pipe_sp_w, "%d", pipe_sp[1]);
    sprintf(pipe_gp_r, "%d", pipe_gp[0]);
    sprintf(pipe_gp_w, "%d", pipe_gp[1]);
    sprintf(pipe_pl_r, "%d", pipe_pl[0]);
    sprintf(pipe_pl_w, "%d", pipe_pl[1]);



    // Fill P with data
    argumentP[1] = referencefrequency; 
    argumentP[2] = waitingtime; 
    argumentP[3] = ip; 
    argumentP[4] = portnumber; 
    argumentP[5] = pipe_sp_r; 
    argumentP[6] = pipe_sp_w; 
    argumentP[7] = pipe_gp_r; 
    argumentP[8] = pipe_gp_w; 
    argumentP[9] = pipe_pl_r; 
    argumentP[10] = pipe_pl_w; 
    argumentP[11] = NULL;

    

    // Fill L with data
    argumentL[1] = pipe_pl_r; 
    argumentL[2] = pipe_pl_w; 
    argumentL[3] = NULL;



    // Fill S with data
    argumentS[1] = pipe_sp_r; 
    argumentS[2] = pipe_sp_w; 
    argumentL[3] = NULL;



    // Fill G with data
    argumentG[1] = portnumber;
    argumentG[2] = pipe_gp_r; //read, pipe2
    argumentG[3] = pipe_gp_w; //write, pipe2
    argumentL[4] = NULL;




    // Processes creation
    G_process = fork(); //Creates child process G

    //Check on the fork
    if (G_process < 0)
    {
        perror("ERROR CHILD_G FAILED");
        return -1;
    }



    if (G_process == 0)
    {
        fflush(stdout);
        char *nameG; 
        nameG = (char *)"./G";
        argumentG[0] = nameG;
        execvp(nameG, argumentG); //Start process G execution
    }



    else
    {

        L_process = fork();//Creates child process L

        //Check on the fork
        if (L_process < 0)
        {
            perror("ERROR CHILD_L FAILED");
            return -1;
        }


        if (L_process == 0)
        {
            fflush(stdout);
            char *nameL; 
            nameL = (char *)"./L";
            argumentL[0] = nameL; 
            execvp(nameL, argumentL); //Start process L execution
        }


        else
        {
            S_process = fork(); //Creates child process S



            if (S_process < 0)
            {
                perror("ERROR CHILD_S FAILED");
                return -1;
            }


            if (S_process == 0)
            {
                fflush(stdout);
                char *nameS; 
                nameS = (char *)"./S";
                argumentS[0] = nameS;
                execvp(nameS, argumentS); //Start process S execution
            }


            else  
            {
                P_process = fork(); //Creates child process P


                if (P_process < 0)
                {
                    perror("ERROR CHILD_P FAILED");
                    return -1;
                }


                if (P_process == 0)
                {
                    fflush(stdout);
                    char *nameP; 
                    nameP = (char *)"./P";
                    argumentP[0] = nameP;
                    execvp(nameP, argumentP); //Start process P execution
                }
            }
        }


        

        // Wait for processes
        waitpid(P_process, &status_P_child, 0);
        waitpid(L_process, &status_L_child, 0);
        waitpid(S_process, &status_S_child, 0);
        waitpid(G_process, &status_G_child, 0);




        // Check that each process terminate correctly
        if (status_P_child == 0) 
        {
            printf("\nThe child process P ends correctly\n");
        }

        if (status_P_child == 1)
        {
            printf("\nThe child process P ends with an error\n");
        }




        if (status_L_child == 0) 
        {
            printf("\nThe child process L ends correctly\n");
        }

        if (status_L_child == 1)
        {
            printf("\nThe child process L ends with an error\n");
        }




        if (status_S_child == 0) 
        {
            printf("\nThe child process S ends correctly\n");
        }

        if (status_S_child == 1)
        {
            printf("\nThe child process S ends with an error\n");
        }




        if (status_G_child == 0) 
        {
            printf("\nThe child process G ends correctly\n");
        }

        if (status_G_child == 1)
        {
            printf("\nThe child process G ends with an error\n");
        }




        exit(0);
    }
    return 0;
}     



