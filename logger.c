//
//  Task ventilator
//  Binds PUSH socket to tcp://localhost:5557
//  Sends batch of tasks to workers via that socket
//
#include "zhelpers.h"
#include <unistd.h>
#include <time.h>


//Function for receiving messages and writing to file
void * logger_receiver(void * subscriber);


int main (void) 
{
    printf("\n---> Starting logger...\n");
    fflush(stdout);
    
    void *context = zmq_ctx_new ();
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    
    zmq_connect (subscriber, "tcp://localhost:5562");
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "Server2Logger", 1);
    
    while(1){
        logger_receiver(subscriber);
    }    
    
    zmq_close (subscriber);
    zmq_ctx_destroy (context); 
}

void * logger_receiver(void * subscriber)
{

    char *address = s_recv (subscriber);    // Read envelope with address
    char *contents = s_recv (subscriber);   // Read message contents

    if(strcmp(address,"Server2Logger")==0){
        
        FILE * fp;
        fp = fopen ("log.txt","a");
        
        time_t t = time(NULL);
        struct tm tm = *localtime(&t); 
        char hour [20];
        sprintf(hour, "%.2d:%.2d:%.2d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf ("\nLOGGER: [%s] %s", hour, contents);  
        fflush(stdout);
        
        fprintf (fp, "[%s] %s", hour, contents);
        fclose(fp);
    }
    
    fflush(stdout);
    
    free (address);
    free (contents);
    
    return NULL;
}