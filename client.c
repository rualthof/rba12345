//
//  Task worker
//  Connects PULL socket to tcp://localhost:5557
//  Collects workloads from ventilator via that socket
//  Connects PUSH socket to tcp://localhost:5558
//  Sends results to sink via that socket
//
#include "zhelpers.h"
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

struct arguments_struct {
    void * subscriber;
    int client_id;
    char last_message [250];
};

void client_interrupt_handler(int sig);
void * receive_messages(void * arguments);
void send_message_to_fifo(struct arguments_struct * args, const char * fifo_name);

//Struct to pass data to thread and to signal handler (that is why it is global)
struct arguments_struct args;

int main (void) 
{
    
    //Creating unique ID for this client
    time_t t;
    srand((unsigned) time(&t));
    int myID = (unsigned) t % 10000;
    
    signal(SIGINT, client_interrupt_handler);

    
    //Socket
    void *context = zmq_ctx_new ();    
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://localhost:5562");
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "Server2Client", 1); 
    
    
    //Thread for receiving data
    pthread_t recv_thread;
    
    //Struct to pass data to thread
    args.client_id = myID;
    args.subscriber = subscriber;
    
    
    if(pthread_create(&recv_thread, NULL, receive_messages, &args)) {    
        fprintf(stderr, "Error creating thread\n");
        return 1;    
    }
    
    //Creates formated message
    sprintf(args.last_message, "===> CREATED client_%d \n", myID);
    printf("%s", args.last_message);
    send_message_to_fifo(&args, "myfifo");
    
    
    printf("\nType something and press enter to send..\n");
    fflush(stdout);
    
    
    
    while(1){
        
        //Reads message from user
        char buffer [300];
        fflush(stdin);
        fgets(buffer, sizeof(buffer), stdin);
        
        int size = (int)strlen(buffer);
        
        //printf("len: %d", size);
        if(size==1){
            printf("Empty message, not sent.\n");
            continue;
        }

        
        //Creates formated message
        sprintf(args.last_message, "client_%d says: %s ", myID, buffer);
        
        //Checks if pipe exists (means that the server is running)
        send_message_to_fifo(&args, "myfifo");
        

    }

    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    
    
    //Wait fo threading to finish
    if(pthread_join(recv_thread, NULL)) {    
        fprintf(stderr, "Error joining thread\n");
        return 2;    
    }
    
    return 0;
}

void * receive_messages(void * arguments)
{
    
    printf("\nCLIENT: receive_messages thread starting..\n");
    fflush(stdout);
   
    //Casting void ponter to struct pointer to access data
    struct arguments_struct * args = (struct arguments_struct *)arguments;
    
    while (1) {

        char *topic = s_recv (args->subscriber);     // Read address (topic)
        char *contents = s_recv (args->subscriber);  // Read contents

        
        //printf ("\n[%s] %s", topic, contents);
        
        //If receive a message from server
        if(strcmp(topic,"Server2Client")==0){
            //if is the same message it sent
            if(strcmp(args->last_message, contents)==0){
                printf("..........sent.\n");
            }            
            //if it is a message from other client, prints more to the right
            else {
                printf("\n%20s%s", "", contents);            
            }           

        }
        
        fflush(stdout);
        free (topic);
        free (contents);
    }    
}

void send_message_to_fifo(struct arguments_struct * args, const char * fifo_name){
    
    //Checks if pipe exists (means that the server is running)
    int fd = open(fifo_name, O_WRONLY/* | O_NONBLOCK*/);
    if(fd>=0){
        write(fd, args->last_message, sizeof(args->last_message) );
        close(fd);
    }
    else {
        printf("....... server offline, cannot send message.\n");
    }
}

void client_interrupt_handler(int sig) {
  printf("Received kb interrupt\n");
  sprintf(args.last_message, "===> DESTROYED client_%d \n", args.client_id);
  send_message_to_fifo(&args, "myfifo");  
  exit(0);
}