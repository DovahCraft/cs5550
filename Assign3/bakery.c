#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

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
  pthread_cond_t *putDownMitts;
  pthread_mutex_t *leftMutex;
  pthread_mutex_t *rightMutex;
  baker_type bakerType;
  int bakerId;
  char *bakerStr;
};

void random_sleep(double a, double b);
void prepareCookies(char *bakerStr, int bakerId);
void waitForCookies(char *bakerStr, int bakerId);
void printArg(struct arguments args);
void grabMitts(void *args);

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
    fprintf(stderr, "Usage: %s <# left-handed bakers> <# right-handed bakers> <# cautious bakers> <seed>\n", argv[0]);
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
    fprintf(stderr, "Invalid command-line arguments... Aborting\n");
    exit(1);
  }

  /* Seed the RNG */
  srand(seed);

  // IMPLEMENT CODE HERE
  // Create batches of workers
  int totalBakers = num_left_handed_bakers + num_right_handed_bakers + num_cautious_bakers;
  pthread_t left_handed_bakers[num_left_handed_bakers];
  pthread_t right_handed_bakers[num_right_handed_bakers];
  pthread_t cautious_bakers[num_cautious_bakers];
  struct arguments threadArgs[totalBakers];
  // Signals to check for mitts again instead of busy waiting.
  pthread_cond_t putDownMitts;
  // Not sure if this is needed.
  pthread_cond_t grabbedMitts;
  pthread_mutex_t leftLock;
  pthread_mutex_t rightLock;
  int numLeftMitts = 3;
  int numRightMitts = 3;
  int currLeftId = 0;
  int currRightId = 0;
  int currCautiousId = 0;

  for (int i = 0; i < totalBakers; i++)
  {
    threadArgs[i].leftMutex = &leftLock;
    threadArgs[i].rightMutex = &rightLock;
    // Build left bakers
    if (currLeftId < num_left_handed_bakers)
    {
      threadArgs[i].bakerType = LEFT_HANDED;
      threadArgs[i].bakerStr = "Left-handed Baker";
      threadArgs[i].bakerId = currLeftId;
      currLeftId++;
    }
    // Then right bakers
    else if (currRightId < num_right_handed_bakers)
    {
      threadArgs[i].bakerType = RIGHT_HANDED;
      threadArgs[i].bakerStr = "Right-handed Baker";
      threadArgs[i].bakerId = currRightId;
      currRightId++;
    }
    // Then finally cautious
    else
    {
      threadArgs[i].bakerType = CAUTIOUS;
      threadArgs[i].bakerStr = "Cautious Baker";
      threadArgs[i].bakerId = currCautiousId;
      currCautiousId++;
    }
    // Shared args
    threadArgs[i].numLeftMitts = &numLeftMitts;
    threadArgs[i].numRightMitts = &numRightMitts;
    // threadArgs[i]->bufferIndex = 0;
    printArg(threadArgs[i]);
  }
}

void printArg(struct arguments args)
{
  struct arguments threadArg = args;
  fprintf(stderr, "Printing args for [%s %d]\n", threadArg.bakerStr, threadArg.bakerId);
  printf("Val: %d\n", threadArg.bakerId);
}

// Driver function for each thread to run
void startBaking(void *args)
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
  }
}

// TODO: Check lock pointers n shit in args.
void grabMitts(void *args)
{
  struct arguments *threadArgs = (struct arguments *)args;
  pthread_mutex_t *leftLock = threadArgs->leftMutex;
  pthread_mutex_t *rightLock = threadArgs->rightMutex;
  pthread_cond_t *putDownMitts = threadArgs->putDownMitts;
  // While loop with cond_wait for mitts.
  // If left baker || cautious baker
  if (threadArgs->bakerType == LEFT_HANDED || threadArgs->bakerType == CAUTIOUS)
  {
    pthread_mutex_lock(leftLock);
    while (threadArgs->numLeftMitts == 0)
    {
      pthread_cond_wait(putDownMitts, leftLock);
    }
    pthread_mutex_unlock(leftLock);
  }

  // If right baker || cautious baker
  if (threadArgs->bakerType == RIGHT_HANDED || threadArgs->bakerType == CAUTIOUS)
  {
    pthread_mutex_lock(rightLock);
    while (threadArgs->numRightMitts == 0)
    {
      pthread_cond_wait(putDownMitts, rightLock);
    }
    pthread_mutex_unlock(rightLock);
  }
}

void putDownMitts(void *args)
{
  struct arguments *threadArgs = (struct arguments *)args;
}

void prepareCookies(char *bakerStr, int bakerId)
{
  fprintf(stderr, "Baker %d preparing the cookies for the oven.", bakerId);
  // Sleep (work) for .2 to .5 seconds
  random_sleep(.2, .5);
  fprintf(stderr, "Baker %d finished preparing the cookies for the oven.", bakerId);
}

void waitForCookies(char *bakerStr, int bakerId)
{
  fprintf(stderr, "[%s %d] has put cookies in the oven and is waiting...", bakerStr, bakerId);
  // Sleep (work) for .2 to .5 seconds
  random_sleep(.2, .5);
  fprintf(stderr, "[%s %d] has taken cookies out of the oven...", bakerStr, bakerId);
}