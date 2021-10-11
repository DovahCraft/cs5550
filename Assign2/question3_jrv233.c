#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void clearBuffer(int *bufferPtr);
int atomicUpdateFunction(pthread_mutex_t *mutexFn, int *totalCountPtr, int goal);
void *do_work(void *arg);

/*
 Problem 3: Ordered execution
*/
//Create argument struct
struct arguments
{
    int tid;
    pthread_mutex_t *mutex;
    int *currTid;
    //Indicates we are looking for the next thread to executre.
    pthread_cond_t *findingThread;
    int *count;
};

void printArg(void *arguments)
{
    struct arguments *threadArg = (struct arguments *)arguments;
    // printf("Val: %d\n", *(threadArg->currTid));
}
int main(int argc, char *argv)
{
    //Make array of three arguments
    int size = 10;
    int i;
    int currTid = 0;
    int count = 0;
    struct arguments *threadArgs[size];
    pthread_t threads[size];
    pthread_mutex_t mutex;
    pthread_cond_t findingThread;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&findingThread, NULL);

    //Initialize arguments struct
    for (i = 0; i < size; i++)
    {
        threadArgs[i] = (struct arguments *)calloc(1, sizeof(struct arguments));

        //Don't use a ++ here! increments i
        threadArgs[i]->tid = i;
        threadArgs[i]->currTid = &currTid;
        threadArgs[i]->count = &count;
        threadArgs[i]->mutex = &mutex;
        threadArgs[i]->findingThread = &findingThread;
        printArg(threadArgs[i]);
    }

    for (i = 0; i < size; i++)
    {
        if (pthread_create(&(threads[i]), NULL,
                           do_work, (void *)threadArgs[i]))
        {
            printf( "Error while joining with child thread #%d\n", i);
            exit(1);
        }
    }

    for (i = 0; i < size; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            printf( "Error while joining with child thread #%d\n", i);
            exit(1);
        }
    }
}

void *do_work(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    int tid = threadArgs->tid;
    int *nextTid = threadArgs->currTid;
    pthread_mutex_t *mutex = threadArgs->mutex;
    pthread_cond_t *findingThread = threadArgs->findingThread;
    int goal = 990;
    int *currCount = threadArgs->count;
    printf( "Starting thread #%d\n", tid);
    //printf( "Thread with val: %d\n", threadArgs->val);
    while (true)
    {
        pthread_mutex_lock(mutex);
        while (*nextTid != tid)
        {
            pthread_cond_wait(findingThread, mutex);
        }

        //printf( "Thread #%d entered increment code.\n", tid);
        //Don't run if we are ready to quit.
        if (*currCount == goal)
        {
            //printf( "Thread #%d entered finishGoal.\n", tid);
            *nextTid += 1;
            pthread_mutex_unlock(mutex);
            pthread_cond_broadcast(findingThread);
            return NULL;
        }
        //Add to our count and signal the next thread.
        *(threadArgs->count) += tid;
        printf( "my num: %d, total: %d\n", tid, *currCount);
        if (tid == 9)
        {
            *nextTid = 0;
        }
        else
        {
            *nextTid += 1;
        }
        if(*currCount == goal){
            printf("Total: %d\n", goal);
        }

        pthread_mutex_unlock(mutex);
        pthread_cond_broadcast(findingThread);
    }
    return NULL;
}
