#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

typedef enum
{
  LEFT_HANDED,
  RIGHT_HANDED,
  CAUTIOUS
} baker_type;

struct arguments
{
  int *numLeftMitts;
  int *numRightMitts;
  int *numIterations;
  pthread_cond_t *putDownLeftMitt;
  pthread_cond_t *putDownRightMitt;
  pthread_mutex_t *leftMutex;
  pthread_mutex_t *rightMutex;
  baker_type bakerType;
  int bakerId;
  char *bakerStr;
  double waitTime;
  // Shared values for waitTime.
  double *leftTotalWait;
  double *rightTotalWait;
  double *cautiousTotalWait;
  // Lock for adding to the totals.
  pthread_mutex_t *sumMutex;
};

void random_sleep(double a, double b);
void *startBaking(void *args);
void prepareCookies(char *bakerStr, int bakerId);
void waitForCookies(char *bakerStr, int bakerId);
void printArg(struct arguments args);
void grabMitts(void *args);
void putDownMitts(void *args);

#define NUM_ITERATIONS 10
#define NUM_LEFT_OVEN_MITTS 3
#define NUM_RIGHT_OVEN_MITTS 3

// Helper function to sleep a random number of microseconds
// picked between two bounds (provided in seconds)
// pass 0.2 and 0.5 into this function as arguments
void random_sleep(double lbound_sec, double ubound_sec)
{
  int num_usec;
  num_usec = lbound_sec * 1000000 +
             (int)((ubound_sec - lbound_sec) * 1000000 * ((double)(rand()) / RAND_MAX));
  usleep(num_usec);
  return;
}

double getDuration(struct timeval start, struct timeval stop)
{
  return (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
}
/*
 * Main function
 */
int main(int argc, char **argv)
{
  int num_left_handed_bakers;
  int num_right_handed_bakers;
  int num_cautious_bakers;
  int seed;

  /* Process command-line arguments */
  if (argc != 5)
  {
    printf("Usage: %s <# left-handed bakers> <# right-handed bakers> <# cautious bakers> <seed>\n", argv[0]);
    exit(1);
  }

  if ((sscanf(argv[1], "%d", &num_left_handed_bakers) != 1) ||
      (sscanf(argv[2], "%d", &num_right_handed_bakers) != 1) ||
      (sscanf(argv[3], "%d", &num_cautious_bakers) != 1) ||
      (sscanf(argv[4], "%d", &seed) != 1) ||
      (num_left_handed_bakers < 1) ||
      (num_right_handed_bakers < 1) ||
      (num_cautious_bakers < 1) ||
      (seed < 0))
  {
    printf("Invalid command-line arguments... Aborting\n");
    exit(1);
  }

  /* Seed the RNG */
  srand(seed);

  // IMPLEMENT CODE HERE
  // Create batches of workers
  int totalBakers = num_left_handed_bakers + num_right_handed_bakers + num_cautious_bakers;
  pthread_t baker_threads[totalBakers];
  struct arguments threadArgs[totalBakers];

  // Signals to check for mitts again instead of busy waiting.
  pthread_cond_t putDownLeftMitt;
  pthread_cond_t putDownRightMitt;

  // Lock both types of mitts
  pthread_mutex_t leftLock;
  pthread_mutex_t rightLock;
  pthread_cond_init(&putDownLeftMitt, NULL);
  pthread_cond_init(&putDownRightMitt, NULL);
  pthread_mutex_init(&leftLock, NULL);
  pthread_mutex_init(&rightLock, NULL);

  // Init arguments
  int numLeftMitts = 3;
  int numRightMitts = 3;
  int currLeftId = 0;
  int currRightId = 0;
  int currCautiousId = 0;

  // Total sums of baker wait times for average.
  double leftTotalWait = 0;
  double rightTotalWait = 0;
  double cautiousTotalWait = 0;

  for (int i = 0; i < totalBakers; i++)
  {
    threadArgs[i].leftMutex = &leftLock;
    threadArgs[i].rightMutex = &rightLock;
    // Build left bakers
    if (currLeftId < num_left_handed_bakers)
    {
      threadArgs[i].bakerType = LEFT_HANDED;
      threadArgs[i].bakerStr = "Left-handed baker";
      threadArgs[i].bakerId = currLeftId;
      currLeftId++;
    }
    // Then right bakers
    else if (currRightId < num_right_handed_bakers)
    {
      threadArgs[i].bakerType = RIGHT_HANDED;
      threadArgs[i].bakerStr = "Right-handed baker";
      threadArgs[i].bakerId = currRightId;
      currRightId++;
    }
    // Then finally cautious
    else
    {
      threadArgs[i].bakerType = CAUTIOUS;
      threadArgs[i].bakerStr = "Cautious baker";
      threadArgs[i].bakerId = currCautiousId;
      currCautiousId++;
    }
    // Shared args
    threadArgs[i].numLeftMitts = &numLeftMitts;
    threadArgs[i].numRightMitts = &numRightMitts;
    threadArgs[i].putDownLeftMitt = &putDownLeftMitt;
    threadArgs[i].putDownRightMitt = &putDownRightMitt;
    threadArgs[i].leftTotalWait = &leftTotalWait;
    threadArgs[i].rightTotalWait = &rightTotalWait;
    threadArgs[i].cautiousTotalWait = &cautiousTotalWait;
    threadArgs[i].waitTime = 0;
  }

  // Create baker threads
  for (int i = 0; i < totalBakers; i++)
  {
    if (pthread_create(&(baker_threads[i]), NULL,
                       startBaking, (void *)&threadArgs[i]))
    {
      printf("Error while joining with child thread #%d\n", i);
      exit(1);
    }
  }
  // Join waiting for them to finish baking.
  for (int i = 0; i < totalBakers; i++)
  {
    if (pthread_join(baker_threads[i], NULL))
    {
      printf("Error while joining with child thread #%d\n", i);
      exit(1);
    }
  }

  // Print averages
  printf("\n\n==================================\nAVERAGE WAIT TIMES\n=========================================\n\n");
  printf("Left average wait (%f/%d): %f\n", leftTotalWait, num_left_handed_bakers, leftTotalWait / (double)num_left_handed_bakers);
  printf("Right average wait (%f/%d): %f\n", rightTotalWait, num_right_handed_bakers, rightTotalWait / (double)num_right_handed_bakers);
  printf("Cautious average wait (%f/%d): %f\n", cautiousTotalWait, num_cautious_bakers, cautiousTotalWait / (double)num_cautious_bakers);
}

void printArg(struct arguments args)
{
  struct arguments threadArg = args;
  printf("Printing args for [%s %d]\n", threadArg.bakerStr, threadArg.bakerId);
  printf("Val: %d\n", threadArg.bakerId);
}

// Driver function for each thread to run
void *startBaking(void *args)
{
  struct arguments *threadArgs = (struct arguments *)args;
  int bakingIterations = NUM_ITERATIONS;
  int bakerId = threadArgs->bakerId;
  baker_type bakerType = threadArgs->bakerType;
  char *bakerStr = threadArgs->bakerStr;

  for (int i = 0; i < bakingIterations; i++)
  {
    prepareCookies(bakerStr, bakerId);
    grabMitts(threadArgs);
    waitForCookies(bakerStr, bakerId);
    putDownMitts(threadArgs);
  }
  // Print out our wait time
  printf("[%s %d] WAIT-TIME = %f....\n", bakerStr, bakerId, threadArgs->waitTime);
}

void grabMitts(void *args)
{
  struct arguments *threadArgs = (struct arguments *)args;
  pthread_mutex_t *leftLock = threadArgs->leftMutex;
  pthread_mutex_t *rightLock = threadArgs->rightMutex;
  pthread_cond_t *putDownLeftMitt = threadArgs->putDownLeftMitt;
  pthread_cond_t *putDownRightMitt = threadArgs->putDownRightMitt;
  char *bakerStr = threadArgs->bakerStr;
  int bakerId = threadArgs->bakerId;
  struct timeval start_t, end_t;
  double diff_t;
  double currDuration;

  // If left baker || cautious baker
  if (threadArgs->bakerType == LEFT_HANDED || threadArgs->bakerType == CAUTIOUS)
  {
    printf("[%s %d] wants a left-handed mitt...\n", bakerStr, bakerId);
    pthread_mutex_lock(leftLock);
    // Start
    gettimeofday(&start_t, NULL);
    //  While loop with cond_wait for mitts.
    while (*(threadArgs->numLeftMitts) == 0)
    {
      // Start the timer
      pthread_cond_wait(putDownLeftMitt, leftLock);
    }
    // Stop
    gettimeofday(&end_t, NULL);
    // Get the wait time
    currDuration = getDuration(start_t, end_t);
    // Add to our total
    threadArgs->waitTime += currDuration;
    // Add to combined total for average calc
    if (threadArgs->bakerType == LEFT_HANDED)
    {
      *(threadArgs->leftTotalWait) += currDuration;
    }
    else
    {
      *(threadArgs->cautiousTotalWait) += currDuration;
    }
    // Grab the mitt
    *(threadArgs->numLeftMitts) -= 1;
    printf("[%s %d] has got a left-handed mitt...\n", bakerStr, bakerId);
    pthread_mutex_unlock(leftLock);
  }

  // If right baker || cautious baker
  if (threadArgs->bakerType == RIGHT_HANDED || threadArgs->bakerType == CAUTIOUS)
  {
    printf("[%s %d] wants a right-handed mitt...\n", bakerStr, bakerId);
    pthread_mutex_lock(rightLock);
    // Start
    gettimeofday(&start_t, NULL);
    while (*(threadArgs->numRightMitts) == 0)
    {
      pthread_cond_wait(putDownRightMitt, rightLock);
    }
    // Stop
    gettimeofday(&end_t, NULL);
    // Get the wait time
    currDuration = getDuration(start_t, end_t);
    // Add to our total
    threadArgs->waitTime += currDuration;
    // Add to combined total for average calc
    if (threadArgs->bakerType == RIGHT_HANDED)
    {
      *(threadArgs->rightTotalWait) += currDuration;
    }
    else
    {
      *(threadArgs->cautiousTotalWait) += currDuration;
    }
    // Grab the mitt.
    *(threadArgs->numRightMitts) -= 1;
    printf("[%s %d] has got a right-handed mitt...\n", bakerStr, bakerId);
    pthread_mutex_unlock(rightLock);
  }
}

void putDownMitts(void *args)
{
  struct arguments *threadArgs = (struct arguments *)args;
  pthread_mutex_t *leftLock = threadArgs->leftMutex;
  pthread_mutex_t *rightLock = threadArgs->rightMutex;
  pthread_cond_t *putDownLeftMitt = threadArgs->putDownLeftMitt;
  pthread_cond_t *putDownRightMitt = threadArgs->putDownRightMitt;
  char *bakerStr = threadArgs->bakerStr;
  int bakerId = threadArgs->bakerId;
  if (threadArgs->bakerType == LEFT_HANDED || threadArgs->bakerType == CAUTIOUS)
  {
    pthread_mutex_lock(leftLock);
    // Grab the mitt
    *(threadArgs->numLeftMitts) += 1;
    printf("[%s %d] has put back a left-handed mitt... \n", bakerStr, bakerId);
    pthread_cond_signal(putDownLeftMitt);
    pthread_mutex_unlock(leftLock);
  }

  // If right baker || cautious baker
  if (threadArgs->bakerType == RIGHT_HANDED || threadArgs->bakerType == CAUTIOUS)
  {
    pthread_mutex_lock(rightLock);
    // Grab the mitt
    *(threadArgs->numRightMitts) += 1;
    printf("[%s %d] has put back a right-handed mitt... \n", bakerStr, bakerId);
    pthread_cond_signal(putDownRightMitt);
    pthread_mutex_unlock(rightLock);
  }
}

void prepareCookies(char *bakerStr, int bakerId)
{
  printf("[%s %d] is working...\n", bakerStr, bakerId);
  // Sleep (work) for .2 to .5 seconds
  random_sleep(.2, .5);
}

void waitForCookies(char *bakerStr, int bakerId)
{
  printf("[%s %d] has put cookies in the oven and is waiting...\n", bakerStr, bakerId);
  // Sleep (work) for .2 to .5 seconds
  random_sleep(.2, .5);
  printf("[%s %d] has taken cookies out of the oven...\n", bakerStr, bakerId);
}