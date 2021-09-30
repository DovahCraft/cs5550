#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void clearBuffer(int *bufferPtr);
void *do_work(void *arg);

/*
 Problem 3: Ordered execution
*/

//Create argument struct
struct arguments
{
    pthread_mutex_t *mutex;
    int *currTid;
    int tid;
};

void printArg(void *arguments)
{
    struct arguments *threadArg = (struct arguments *)arguments;
    printf("Val: %d\n", *(threadArg->currTid));
}
int main(int argc, char *argv)
{
    //Make array of three arguments
    int size = 10;
    struct arguments *threadArgs[size];
    pthread_t threads[10];
    int i;
    int currTid = 0;
    pthread_mutex_t countLock;
    pthread_mutex_init(&countLock, NULL);

    //Initialize arguments struct
    for (i = 0; i < size; i++)
    {
        threadArgs[i] = (struct arguments *)calloc(1, sizeof(struct arguments));

        //Don't use a ++ here! increments i
        threadArgs[i]->tid = i;
        threadArgs[i]->currTid = &currTid;
        threadArgs[i]->mutex = &countLock;
        printArg(threadArgs[i]);
    }

    for (i = 0; i < size; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            fprintf(stderr, "Error while joining with child thread #%d\n", i);
            exit(1);
        }
    }
}

void clearBuffer(int *bufferPtr)
{
    int size = 3;
    for (int i = 0; i < size; i++)
    {
        *bufferPtr = 0;
        bufferPtr++;
    }
}

bool createdSequence1(int buffer1[])
{
    return (buffer1[0] == 1 && buffer1[1] == 2 && buffer1[2] == 3);
}

bool createdSequence2(int buffer2[])
{
    return (buffer2[0] == 4 && buffer2[1] == 5 && buffer2[2] == 6);
}

bool printBuffer(int buffer1[])
{
    fprintf(stderr, "\nBuffer: [%d,%d,%d]\n", buffer1[0], buffer1[1], buffer1[2]);
}

void *do_work(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    int tid = threadArgs->tid;
    pthread_mutex_t *mutex = threadArgs->mutex;
    //fprintf(stderr, "Thread with val: %d\n", threadArgs->val);

    return NULL;
}
