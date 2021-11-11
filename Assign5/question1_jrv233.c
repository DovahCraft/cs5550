//compilation instructions/examples:
//gcc -fopenmp point_epsilon_starter.c -o point_epsilon_starter
//sometimes you need to link against the math library with -lm:
//gcc -fopenmp point_epsilon_starter.c -lm -o point_epsilon_starter

//math library needed for the square root

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "omp.h"

//N is 100000 for the submission. However, you may use a smaller value of testing/debugging.
#define N 100000
//Do not change the seed, or your answer will not be correct
#define SEED 72

struct pointData
{
	double x;
	double y;
};
double computeEuclidDist(struct pointData *p1, struct pointData *p2);
void generateDataset(struct pointData *data);

int main(int argc, char *argv[])
{
	//Read epsilon distance from command line
	if (argc != 2)
	{
		printf("\nIncorrect number of input parameters. Please input an epsilon distance.\n");
		return 0;
	}

	char inputEpsilon[20];
	strcpy(inputEpsilon, argv[1]);
	double epsilon = atof(inputEpsilon);
	int count = 0;

	//generate dataset:
	struct pointData *data;
	data = (struct pointData *)malloc(sizeof(struct pointData) * N);
	printf("\nSize of dataset (MiB): %f", (2.0 * sizeof(double) * N * 1.0) / (1024.0 * 1024.0));
	generateDataset(data);

	//change OpenMP settings:
	omp_set_num_threads(6);

	double tstart = omp_get_wtime();

	//Write your code here:
	//The data you need to use is stored in the variable "data",
	//which is of type pointData
	//Sequential dumb code of badness
	/*for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (computeEuclidDist(&(data[i]), &(data[j])) <= epsilon)
			{
				count++;
			}
		}
	}*/

	int i, j = 0;
//Begin BF with omp
/*#pragma omp parallel shared(data, count) private(i, j)
	{
#pragma omp for
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
			{
				if (computeEuclidDist(&(data[i]), &(data[j])) <= epsilon)
				{
#pragma omp atomic
					count++;
					printf("Found valid point pair! Count is now: %d\n", count);
				}
			}
		}
	}*/

//Begin optimized omp algo
#pragma omp parallel shared(data, count) private(i, j)
	{
#pragma omp for reduction(+ \
						  : count)
		for (i = 0; i < N; i++)
		{
#pragma omp atomic
			count += 1;
			for (j = i + 1; j < N; j++)
			{
				if (computeEuclidDist(&(data[i]), &(data[j])) <= epsilon)
				{
#pragma omp atomic
					count += 2;
					printf("Found valid point pair! Count is now: %d\n", count);
				}
			}
		}
	}

	double tend = omp_get_wtime();

	printf("Count: %d", count);
	printf("\nTotal time (s): %f", tend - tstart);

	free(data);
	printf("\n");
	return 0;
}

double computeEuclidDist(struct pointData *p1, struct pointData *p2)
{
	return sqrt(pow((p1->x - p2->x), 2) + pow((p1->y - p2->y), 2));
}

//Do not modify the dataset generator or you will get the wrong answer
void generateDataset(struct pointData *data)
{
	//seed RNG
	srand(SEED);

	for (unsigned int i = 0; i < N; i++)
	{
		data[i].x = 1000.0 * ((double)(rand()) / RAND_MAX);
		data[i].y = 1000.0 * ((double)(rand()) / RAND_MAX);
	}
}
