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

  double tstart = omp_get_wtime();

//modify code here
#pragma omp parallel for private(i, j) shared(mat)
  for (i = 0; i < SIZE; i++)
  { /* loop over the rows */
    for (j = 0; j < SIZE; j++)
    { /* loop over the columns */
      mat[i][j] = do_crazy_computation(i, j);
      fprintf(stderr, ".");
    }
  }
  double tend = omp_get_wtime();
  double elapsed = tend - tstart;
  printf("\nElapsed time: %f seconds\n", elapsed);
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
