#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void clearBuffer(int *bufferPtr);
bool createdSequence1(int buffer1[]);
bool createdSequence2(int buffer2[]);
void *do_work(void *arg);
void *do_work2(void *arg);
void printArg(void *arguments);

/*
 Problem 2: Threads racing problem
*/

//Create argument struct
struct arguments
{
    pthread_mutex_t *mutex;
    pthread_mutex_t *finishedLock;
    int val;
    int *bufferIndex;
    int *buffer;
    //Correct number, goal is 10
    int *numCorrect1;
    int *numCorrect2;
    //All the sequences
    int *totalCount1;
    int *totalCount2;
    bool *finishedFlag;
};


int main(int argc, char *argv)
{
    int buffer1[3] = {0, 0, 0};
    int buffer2[3] = {0, 0, 0};
    int bufferIndex1 = 0;
    int bufferIndex2 = 0;
    //Num correct by each team
    int numCorrect1 = 0;
    int numCorrect2 = 0;
    int totalCount1 = 0;
    int totalCount2 = 0;
    bool finishedFlag = false;
    //Make array of three arguments
    int size = 6;
    pthread_t threads[size];
    struct arguments *threadArgs[size];
    int i;
    pthread_mutex_t buffer1Lock;
    pthread_mutex_t buffer2Lock;
    pthread_mutex_t finishedLock;
    pthread_mutex_init(&buffer1Lock, NULL);
    pthread_mutex_init(&buffer2Lock, NULL);
    pthread_mutex_init(&finishedLock, NULL);
    //Initialize arguments struct
    for (i = 0; i < size; i++)
    {
        threadArgs[i] = (struct arguments *)calloc(1, sizeof(struct arguments));
        threadArgs[i]->val = i + 1;      
        threadArgs[i]->numCorrect1 = &numCorrect1;
        threadArgs[i]->numCorrect2 = &numCorrect2;
        threadArgs[i]->totalCount1 = &totalCount1;
        threadArgs[i]->totalCount2 = &totalCount2;
        threadArgs[i]->finishedFlag = &finishedFlag;
        threadArgs[i]->finishedLock = &finishedLock;
        if (i < 3)
        {
            threadArgs[i]->buffer = buffer1;
            threadArgs[i]->bufferIndex = &bufferIndex1;
            threadArgs[i]->mutex = &buffer1Lock;
        }
        else
        {
            threadArgs[i]->buffer = buffer2;
            threadArgs[i]->bufferIndex = &bufferIndex2;
            threadArgs[i]->mutex = &buffer2Lock;
        }
       // printArg(threadArgs[i]);
    }

        for (i = 0; i < size/2; i++)
    {
        if (pthread_create(&(threads[i]), NULL,
                           do_work, (void *)threadArgs[i]))
        {
            printf( "Error while joining with child thread #%d\n", i);
            exit(1);
        }
    }

       for (i = size/2; i < size; i++)
    {
        if (pthread_create(&(threads[i]), NULL,
                           do_work2, (void *)threadArgs[i]))
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

void printArg(void *arguments)
{
    struct arguments *threadArg = (struct arguments *)arguments;
    printf("Val: %d\n", threadArg->val);
    //printf("bufferIndex: %d\n", *(threadArg->bufferIndex));
    printf("bufferVal: %d\n", threadArg->buffer[1]);
    printf("numCorrect1: %d\n", *(threadArg->numCorrect1));
    printf("numCorrect2: %d\n", *(threadArg->numCorrect2));
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
    printf( "\nBuffer: [%d,%d,%d]\n", buffer1[0], buffer1[1], buffer1[2]);
}

void *do_work(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    int tid = threadArgs->val;
    pthread_mutex_t *mutex = threadArgs->mutex;
    //printf( "Thread with val: %d\n", threadArgs->val);
    int *numCorrect1 = threadArgs->numCorrect1;
    int *totalCount1 = threadArgs->totalCount1;
    //Enemy teams scores
    int *numCorrect2 = threadArgs->numCorrect2;
    int *totalCount2 = threadArgs->totalCount2;
    bool *finishedFlag = threadArgs->finishedFlag;
    int *indexPtr = NULL;
    bool createdSequence = false;
    //Loop here while shared num sequences is < 10
    //threadArgs->numCorrect < 10
    while (*numCorrect1 < 10 && !(*finishedFlag))
    {
        //Begin critical section
        pthread_mutex_lock(mutex);
        if (*finishedFlag)
        {
            pthread_mutex_unlock(mutex);
            break;
        }
        indexPtr = threadArgs->bufferIndex;
        printf( "My id: %d\n", tid);
        //Set position in buffer1.
        threadArgs->buffer[*indexPtr] = threadArgs->val;
        if (*indexPtr == 2)
        {
            //Log this attempt
            *totalCount1 += 1;
            //printBuffer(threadArgs->buffer);
            createdSequence = createdSequence1(threadArgs->buffer);
            if (createdSequence)
            {
                printf( "123\n");
                *numCorrect1 += 1;
                //printf( "Count: %d\n", *numCorrect);
                //If we get here we are done, print the final result.
                if (*numCorrect1 == 10)
                {
                    //Set flag to say race won.
                    *finishedFlag = true;
                    printf( "Team 1 won!");
                }
            }
            *indexPtr = 0;
            clearBuffer(threadArgs->buffer);
        }
        else
        {
            *indexPtr += 1;
        }
        //Only print this if you are the winner and the game is over.
        if (*finishedFlag && *numCorrect1 == 10)
        {
            printf( "Total sequences generated team1: %d\n", *(threadArgs->totalCount1));
            printf( "Number of correct sequences team1: %d\n", *(threadArgs->numCorrect1));
            printf( "Total sequences generated team2: %d\n", *(threadArgs->totalCount2));
            printf( "Number of correct sequences team2: %d\n", *(threadArgs->numCorrect2));
        }

        pthread_mutex_unlock(mutex);
        //printf( "Thread %d left crit section with index %d\n", tid, *indexPtr);
        //Sleep for now.
        usleep(500000);
    }

    return NULL;
}

void *do_work2(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    int *numCorrect2 = threadArgs->numCorrect2;
    pthread_mutex_t *mutex = threadArgs->mutex;
    int tid = threadArgs->val;
    int *totalCount2 = threadArgs->totalCount2;
    bool *finishedFlag = threadArgs->finishedFlag;
    int *indexPtr = NULL;
    bool createdSequence = false;

    //Loop here while shared num sequences is < 10
    //threadArgs->numCorrect < 10
    while (*numCorrect2 < 10 && !(*finishedFlag))
    {
        //printf( "\nThread %d entered do_work2 loop\n", tid);
        //Begin critical section
        pthread_mutex_lock(mutex);
        if (*finishedFlag)
        {
            pthread_mutex_unlock(mutex);
            break;
        }
        indexPtr = threadArgs->bufferIndex;
        printf( "My id: %d\n", tid);
        //Set position in buffer1.
        threadArgs->buffer[*indexPtr] = threadArgs->val;
        if (*indexPtr == 2)
        {
            //Log this attempt
            *totalCount2 += 1;
           // printBuffer(threadArgs->buffer);
            createdSequence = createdSequence2(threadArgs->buffer);
            if (createdSequence)
            {
                printf( "456\n");
                *numCorrect2 += 1;
                //printf( "Count: %d\n", *numCorrect);
                //If we get here we are done, print the final result.
                if (*numCorrect2 == 10)
                {
                    printf( "Team 2 won!");
                    *finishedFlag = true;
                }
            }
            *indexPtr = 0;
            clearBuffer(threadArgs->buffer);
        }
        else
        {
            *indexPtr += 1;
        }
        //Only print this if you are the winner and the game is over.
        if (*finishedFlag && *numCorrect2 == 10)
        {
            printf( "Total sequences generated team1: %d\n", *(threadArgs->totalCount1));
            printf( "Number of correct sequences team1: %d\n", *(threadArgs->numCorrect1));
            printf( "Total sequences generated team2: %d\n", *(threadArgs->totalCount2));
            printf( "Number of correct sequences team2: %d\n", *(threadArgs->numCorrect2));
        }
        pthread_mutex_unlock(mutex);
        //Sleep for now.
        usleep(500000);
    }

    return NULL;
}
