#include "zhelpers.h"
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

//Handler for keyboard interrupt, removing the fifo create during the program
void server_interrupt_handler(int sig);


int main (void) 
{
    printf("\n---> Starting server...\n");
    fflush(stdout);
                
    signal(SIGINT, server_interrupt_handler);
    
    void *context = zmq_ctx_new ();
    void *publisher = zmq_socket (context, ZMQ_PUB);
    zmq_bind (publisher, "tcp://*:5562");
        
    char * myfifo = "myfifo";        
    mkfifo(myfifo, 0666); 
    
    while(1){        
        
        int fd = open(myfifo, O_RDONLY);
        if(fd>=0){
            char string [500];
            read(fd, string, 500);
            close(fd);
            
            int size = (int) strlen(string);        
            if (size>0){
            
                printf("\nSERVER: %s", string);
                fflush(stdout);
                
                //Send message to logger
                s_sendmore (publisher, "Server2Logger");
                s_send (publisher, string);
                
                //Distribute message to all clientes
                s_sendmore (publisher, "Server2Client");
                s_send (publisher, string);   
            }
            else {
                printf("\nSERVER: Empty message");
            }

        }
        else {
            printf("/nSERVER: Could not create pipe file...");
        }
        
        fflush(stdout);
    }
    
    unlink(myfifo);
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}

void server_interrupt_handler(int sig) {
  printf("Received kb interrupt\n");
  unlink("myfifo");
  exit(0);
}
