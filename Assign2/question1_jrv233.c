#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void clearBuffer(int *bufferPtr);
bool createdSequence1(int buffer[]);
void *do_work(void *arg);
/*
 Problem 1: Generating a sequence using pthreads
*/

//Create argument struct
struct arguments
{
    pthread_mutex_t *mutex;
    int val;
    int bufferIndex;
    int *buffer;
    int *count;
    int numCorrect;
};

void printArg(void *arguments)
{
    struct arguments *threadArg = (struct arguments *)arguments;
    printf("Val: %d\n", threadArg->val);
    printf("bufferIndex: %d\n", threadArg->bufferIndex);
    printf("bufferVal: %d\n", threadArg->buffer[1]);
    printf("numCorrect: %d\n", threadArg->numCorrect);
}
int main(int argc, char *argv)
{
    pthread_t thread0;
    pthread_t thread1;
    pthread_t thread2;
    int buffer[3] = {0, 1, 0};
    int count = 0;
    //Make array of three arguments
    int size = 3;
    struct arguments *threadArgs[3];
    int i;
    pthread_mutex_t buffer1Lock;
    pthread_mutex_init(&buffer1Lock, NULL);
    //Initialize arguments struct
    for (i = 0; i < size; i++)
    {
        threadArgs[i] = (struct arguments *)calloc(1, sizeof(struct arguments));
        threadArgs[i]->mutex = &buffer1Lock;
        //Don't use a ++ here! increments i
        threadArgs[i]->val = i + 1;
        threadArgs[i]->buffer = buffer;
        threadArgs[i]->count = &count;
        //threadArgs[i]->bufferIndex = 0;
        printArg(threadArgs[i]);
    }

    //Now that we have three sets of arguments, pass them to start the threads.
    if (pthread_create(&thread0, NULL,
                       do_work, (void *)threadArgs[0]))
    {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }
    if (pthread_create(&thread1, NULL,
                       do_work, (void *)threadArgs[1]))
    {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }
    if (pthread_create(&thread2, NULL,
                       do_work, (void *)threadArgs[2]))
    {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }

    //Wait for threads to finish with join
    // Join with thread
    if (pthread_join(thread0, NULL))
    {
        fprintf(stderr, "Error while joining with child thread\n");
        exit(1);
    }
    if (pthread_join(thread1, NULL))
    {
        fprintf(stderr, "Error while joining with child thread\n");
        exit(1);
    }
    // Join with thread
    if (pthread_join(thread2, NULL))
    {
        fprintf(stderr, "Error while joining with child thread\n");
        exit(1);
    }
}

void clearBuffer(int *bufferPtr)
{
    // printf("Val from clear_buffer: %d", bufferPtr[1]);
    int size = 3;
    for (int i = 0; i < size; i++)
    {
        *bufferPtr = 0;
        bufferPtr++;
    }
}

bool createdSequence1(int buffer[])
{
    return (buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3);
}

void *do_work(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    pthread_mutex_t *mutex = threadArgs->mutex;
    fprintf(stderr, "Thread with val: %d\n", threadArgs->val);
    int *count = NULL;

    //Loop here while shared num sequences is < 10
    //threadArgs->numCorrect < 10
    while (*count < 10)
    {
        //Critical section
        pthread_mutex_lock(mutex);

        (*count)++;
        fprintf(stderr, "Thread %d entered critical section and made count equal %d\n", threadArgs->val, *count);

        if ((*count) == 2)
        {
            fprintf(stderr, "Two achieved");
        }

        //Set our position
        pthread_mutex_unlock(mutex);
        fprintf(stderr, "Thread %d left critical section\n", threadArgs->val);
        usleep(200000);

        //Check the value of index, do we have a complete sequence?
    }
    printf("Ten achieved in thread %d", threadArgs->val);
    return NULL;
}