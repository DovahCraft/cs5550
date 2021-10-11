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
    int goalIterations;
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
    int goalIterations = 4;
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
        threadArgs[i]->goalIterations = goalIterations;
        //printArg(threadArgs[i]);
    }

    if (pthread_create(&(threads[0]), NULL,
                       increment_work, (void *)threadArgs[0]))
    {
        printf( "Error while joining with child thread #%d\n", i);
        exit(1);
    }

    if (pthread_create(&(threads[1]), NULL,
                       decrement_work, (void *)threadArgs[1]))
    {
        printf( "Error while joining with child thread #%d\n", i);
        exit(1);
    }

    if (pthread_join(threads[0], NULL))
    {
        printf( "Error while joining with child thread #%d\n", i);
        exit(1);
    }

    if (pthread_join(threads[1], NULL))
    {
        printf( "Error while joining with child thread #%d\n", i);
        exit(1);
    }
}

//Checks to see if we made it to the goal count yet. While loop condition
int atomicUpdateFunction(pthread_mutex_t *mutexFn, int *pingIterationsPtr, int goalIterations)
{
    int retval = 0;
    pthread_mutex_lock(mutexFn);
    if (*pingIterationsPtr <= goalIterations)
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
    int goalIterations = threadArgs->goalIterations;
    while (atomicUpdateFunction(mutex, pingIterations, goalIterations))
    {
        pthread_mutex_lock(mutex);
        //We finished.
        if(*pingIterations == goalIterations){
            pthread_cond_signal(switchingDirection);
            pthread_mutex_unlock(mutex);
            return NULL;
        }
        //printf( "Curr count in decrement: %d", *currCount);
        while (*currCount != 10)
        {
            //printf( "Thread #%d waiting to decrement\n", tid);
            pthread_cond_wait(switchingDirection, mutex);
        }
        while (*currCount > 0)
        {
            (*currCount)--;
            printf( "Count is now (dec fn): %d\n", *currCount);
        }
        //Mark this ping it
        (*pingIterations)++;
        pthread_cond_signal(switchingDirection);
        pthread_mutex_unlock(mutex);

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
    int goalIterations = threadArgs->goalIterations;
    while (atomicUpdateFunction(mutex, pingIterations, goalIterations))
    {
        pthread_mutex_lock(mutex);
        //We finished.
        if(*pingIterations == goalIterations){
            pthread_cond_signal(switchingDirection);
            pthread_mutex_unlock(mutex);
            return NULL;
        }
        //printf( "Curr count in increment: %d", *currCount);
        while (*currCount != 0)
        {
            //printf( "Thread #%d waiting to increment\n", tid);
            pthread_cond_wait(switchingDirection, mutex);
        }
        while (*currCount < 10)
        {
            (*currCount)++;
            printf( "Count is now (inc fn): %d\n", *currCount);
        }
        //Mark this ping it
        (*pingIterations)++;
        pthread_cond_signal(switchingDirection);
        pthread_mutex_unlock(mutex);

    }
    return NULL;
}