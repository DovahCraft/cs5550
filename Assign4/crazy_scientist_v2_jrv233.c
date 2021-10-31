#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define SIZE 50
#define NUM_THREADS 2

double do_crazy_computation(int i, int j);

int main(int argc, char **argv)
{
  double mat[SIZE][SIZE];
  int i, j;
  //Set two threads
  omp_set_num_threads(NUM_THREADS);

  double tstart = 0;
  double tend = 0;
  double tid = 0;
  double elapsed = 0;
  double t0elapsed = 0;
  double t1elapsed = 0;
  double totalStart = omp_get_wtime();

//modify code here
#pragma omp parallel private(tstart, tend, i, j, elapsed, tid) shared(mat)
  {
    tstart = omp_get_wtime();
    tid = omp_get_thread_num();
#pragma omp for nowait
    for (i = 0; i < SIZE; i++)
    { /* loop over the rows */
      for (j = 0; j < SIZE; j++)
      { /* loop over the columns */
        mat[i][j] = do_crazy_computation(i, j);
        fprintf(stderr, ".");
      }
    }
    tend = omp_get_wtime();
    elapsed = tend - tstart;
    if (tid == 0)
    {
      t0elapsed = elapsed;
    }
    else
    {
      t1elapsed = elapsed;
    }
  }

  double totalEnd = omp_get_wtime();
  double totalElapsed = totalEnd - totalStart;
  printf("\nTotal elapsed time (sanity check): %f\n", totalElapsed);
  printf("Time Thread1: %f\n", t0elapsed);
  printf("Time Thread2: %f\n", t1elapsed);
  double imbalance = fabs(t1elapsed - t0elapsed);
  printf("Load Imbalance: %f\n", imbalance);
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
