#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void clearBuffer(int *bufferPtr);
bool createdSequence1(int buffer[]);
void *do_work(void *arg);
/*
 Problem 1: Generating a sequence using pthreads
*/

//Create argument struct
struct arguments {
    int val;
    int bufferIndex;
};

int buffer[3] = {0,0,0};
//Make array of three arguments
const int BUFFER_SIZE = 3;
struct arguments *arg[3];


int main(int argc, char *argv){
    pthread_t thread0;
    pthread_t thread1;
    pthread_t thread2;
    pthread_mutex_t buffer1Lock;
    int numCorrect = 0;
    int numSequences = 0;
    pthread_mutex_init(&buffer1Lock, NULL);
    arg[0] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[0]->val = 1;
    arg[0]->bufferIndex = 0;
    arg[1] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[1]->val = 2;
    arg[1]->bufferIndex = 1;
    arg[2] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[2]->val = 3;
    arg[2]->bufferIndex=2;

    //Loop for ten tries
    while(numSequences < 10){
        if (pthread_create(&thread0, NULL,
                do_work, (void *)arg[0])) {
            fprintf(stderr,"Error while creating thread\n");
            exit(1);
        }
        if (pthread_create(&thread1, NULL,
                do_work, (void *)arg[1])) {
            fprintf(stderr,"Error while creating thread\n");
            exit(1);
        }
        if (pthread_create(&thread2, NULL,
                do_work, (void *)arg[2])) {
            fprintf(stderr,"Error while creating thread\n");
            exit(1);
        }
        //Create three threads and send them with args

        //Wait for each to finish do_work

        //Print sequence
        if(createdSequence1(buffer)){
            numCorrect++;
            
            printf("numCorrect: %d\n", numCorrect);
        }
        //Log this attempt
        numSequences++;
    }
}

void clearBuffer(int *bufferPtr){
    printf("Val from clear_buffer: %d", buffer[1]);
    for(int i = 0; i <BUFFER_SIZE; i++){
        *bufferPtr = 0;
        bufferPtr++;
    }
}

bool createdSequence1(int buffer[]){
    return (buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3);
    
}

void *do_work(void *arg){
    struct arguments *arguments = (struct arguments*)arg;
    printf("Thread with val: %d", arguments->val);
    return NULL;
}