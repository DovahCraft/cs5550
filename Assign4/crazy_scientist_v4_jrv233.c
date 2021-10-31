#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <omp.h>

#define SIZE 50

int currRow = 0;
pthread_mutex_t mutex;
double mat[SIZE][SIZE];
int numIterations1 = 0;
int numIterations2 = 0;

double t1start = 0;
double t2start = 0;
double t1end = 0;
double t2end = 0;

double t1elapsed = 0;
double t2elapsed = 0;
double do_crazy_computation(int i, int j);
void *compute_rows(void *arg);

int main(int argc, char **argv)
{
  pthread_t worker_thread1;
  pthread_t worker_thread2;
  pthread_mutex_init(&mutex, NULL);

  int i, j;
  int tid1 = 1;
  int tid2 = 2;

  double tstart = omp_get_wtime();

  // Create thread1
  t1start = omp_get_wtime();
  if (pthread_create(&worker_thread1, NULL,
                     compute_rows, (void *)&tid1))
  {
    fprintf(stderr, "Error while creating thread\n");
    exit(1);
  }
  // Create thread2
  t2start = omp_get_wtime();
  if (pthread_create(&worker_thread2, NULL,
                     compute_rows, (void *)&tid2))
  {
    fprintf(stderr, "Error while creating thread\n");
    exit(1);
  }

  // Join with thread
  if (pthread_join(worker_thread1, NULL))
  {
    fprintf(stderr, "Error while joining with child thread\n");
    exit(1);
  }
  if (pthread_join(worker_thread2, NULL))
  {
    fprintf(stderr, "Error while joining with child thread\n");
    exit(1);
  }
  fprintf(stderr, "\nNumber of iterations for thread 1: %d\n", numIterations1);
  fprintf(stderr, "Number of iterations for thread 2: %d\n", numIterations2);
  double tend = omp_get_wtime();
  double elapsed = tend - tstart;
  fprintf(stderr, "Time Thread1: %f\n", t1elapsed);
  fprintf(stderr, "Time Thread2: %f\n", t2elapsed);
  printf("\nTotal Elapsed time: %f seconds\n", elapsed);
  double imbalance = fabs(t1elapsed - t2elapsed);
  printf("Load imbalance: %f", imbalance);

  exit(0);
}

//Crazy computation
double do_crazy_computation(int x, int y)
{
  int iter;
  double value = 0.0;

  for (iter = 0; iter < 5 * x * x * x + 1 + y * y * y + 1; iter++)
  {
    value += (cos(x * value) + sin(y * value));
  }
  return value;
}

//Checks to see if we made it to the goal count yet. While loop condition
int atomicUpdateFunction()
{
  int retval = 0;
  pthread_mutex_lock(&mutex);
  if (currRow < SIZE)
  {
    //printf("\n\ngoal: %d count: %d\n\n", goal, *countClientsPtr);
    retval = 1;
  }
  pthread_mutex_unlock(&mutex);

  return retval;
}

void *compute_rows(void *arg)
{

  int localRow = -1;
  int *tidPtr = (int *)arg;
  int tid = *tidPtr;

  //Check current i
  while (atomicUpdateFunction())
  {
    //Critical section to update the value.
    pthread_mutex_lock(&mutex);
    if (currRow > SIZE)
    {
      break;
    }
    localRow = currRow;
    currRow += 1;
    pthread_mutex_unlock(&mutex);
    //fprintf(stderr, "Thread %d: computing row %d\n", tid, localRow);
    for (int j = 0; j < SIZE; j++)
    { /* loop over the columns */
      mat[localRow][j] = do_crazy_computation(localRow, j);
      fprintf(stderr, ".");
    }
    if (tid == 1)
    {
      numIterations1++;
    }
    else
    {
      numIterations2++;
    }
  }
  //Check outside of while loop to get correct thread timings.
  if (tid == 1)
  {
    t1end = omp_get_wtime();
    t1elapsed = t1end - t1start;
  }
  else
  {
    t2end = omp_get_wtime();
    t2elapsed = t2end - t2start;
  }
  return NULL;
}
