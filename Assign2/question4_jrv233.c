#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void clearBuffer(int *bufferPtr);
int atomicUpdateFunction(pthread_mutex_t *mutexFn, int *pingIterationsPtr, int goal);
void *decrement_work(void *arg);
void *increment_work(void *arg);
/*
 Problem 4: Ping Pong 0-10
*/
//Create argument struct
struct arguments
{
    int tid;
    pthread_mutex_t *mutex;
    //Indicates we are looking to start inc or dec calls.
    pthread_cond_t *switchingDirection;
    int *count;
    int *pingIterations;
};

void printArg(void *arguments)
{
    struct arguments *threadArg = (struct arguments *)arguments;
    // printf("Val: %d\n", *(threadArg->currTid));
}
int main(int argc, char *argv)
{
    //Make array of three arguments
    int size = 2;
    int i;
    int currTid = 0;
    //Set count to 0 to start with.
    int count = 0;
    int pingIterations = 0;
    struct arguments *threadArgs[size];
    pthread_t threads[size];
    pthread_mutex_t mutex;
    pthread_cond_t switchingDirection;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&switchingDirection, NULL);

    //Initialize arguments struct
    for (i = 0; i < size; i++)
    {
        threadArgs[i] = (struct arguments *)calloc(1, sizeof(struct arguments));

        //Don't use a ++ here! increments i
        threadArgs[i]->tid = i;
        threadArgs[i]->count = &count;
        threadArgs[i]->mutex = &mutex;
        threadArgs[i]->switchingDirection = &switchingDirection;
        threadArgs[i]->pingIterations = &pingIterations;
        //printArg(threadArgs[i]);
    }

    if (pthread_create(&(threads[0]), NULL,
                       increment_work, (void *)threadArgs[0]))
    {
        fprintf(stderr, "Error while joining with child thread #%d\n", i);
        exit(1);
    }

    if (pthread_create(&(threads[1]), NULL,
                       decrement_work, (void *)threadArgs[1]))
    {
        fprintf(stderr, "Error while joining with child thread #%d\n", i);
        exit(1);
    }

    if (pthread_join(threads[0], NULL))
    {
        fprintf(stderr, "Error while joining with child thread #%d\n", i);
        exit(1);
    }

    if (pthread_join(threads[1], NULL))
    {
        fprintf(stderr, "Error while joining with child thread #%d\n", i);
        exit(1);
    }
}

//Checks to see if we made it to the goal count yet. While loop condition
int atomicUpdateFunction(pthread_mutex_t *mutexFn, int *pingIterationsPtr, int goal)
{
    int retval = 0;
    pthread_mutex_lock(mutexFn);
    if (*pingIterationsPtr < goal)
    {
        //printf("\n\ngoal: %d count: %d\n\n", goal, *countClientsPtr);
        retval = 1;
    }
    pthread_mutex_unlock(mutexFn);

    return retval;
}

void *decrement_work(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    int tid = threadArgs->tid;
    pthread_mutex_t *mutex = threadArgs->mutex;
    pthread_cond_t *switchingDirection = threadArgs->switchingDirection;

    int *currCount = threadArgs->count;
    int *pingIterations = threadArgs->pingIterations;
    while (atomicUpdateFunction(mutex, pingIterations, 5))
    {
        pthread_mutex_lock(mutex);
        //fprintf(stderr, "Curr count in decrement: %d", *currCount);
        while (*currCount != 10)
        {
            //fprintf(stderr, "Thread #%d waiting to decrement", tid);
            pthread_cond_wait(switchingDirection, mutex);
        }
        while (*currCount > 0)
        {
            (*currCount)--;
            fprintf(stderr, "Count is now (dec fn): %d\n", *currCount);
        }
        //Mark this ping it
        (*pingIterations)++;
        pthread_mutex_unlock(mutex);
        pthread_cond_signal(switchingDirection);
    }
    return NULL;
}

void *increment_work(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    int tid = threadArgs->tid;
    pthread_mutex_t *mutex = threadArgs->mutex;
    pthread_cond_t *switchingDirection = threadArgs->switchingDirection;

    int *currCount = threadArgs->count;
    int *pingIterations = threadArgs->pingIterations;
    while (atomicUpdateFunction(mutex, pingIterations, 5))
    {
        pthread_mutex_lock(mutex);
        //fprintf(stderr, "Curr count in increment: %d", *currCount);
        while (*currCount != 0)
        {
            //fprintf(stderr, "Thread #%d waiting to increment", tid);
            pthread_cond_wait(switchingDirection, mutex);
        }
        while (*currCount < 10)
        {
            (*currCount)++;
            fprintf(stderr, "Count is now (inc fn): %d\n", *currCount);
        }
        //Mark this ping it
        (*pingIterations)++;
        pthread_mutex_unlock(mutex);
        pthread_cond_signal(switchingDirection);
    }
    return NULL;
}