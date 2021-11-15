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
#include <stdbool.h>
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

struct pointNode
{
	struct pointData point;
	struct pointNode *next;
};

double computeEuclidDist(struct pointData p1, struct pointData p2);
void generateDataset(struct pointData *data);
int checkBucket(const struct pointData currentPoint, struct pointNode *pointsToCheck, const double epsilon);

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
	long long int count = 0;

	//generate dataset:
	struct pointData *data;
	data = (struct pointData *)malloc(sizeof(struct pointData) * N);
	printf("\nSize of dataset (MiB): %f ", (2.0 * sizeof(double) * N * 1.0) / (1024.0 * 1024.0));
	generateDataset(data);
	//change OpenMP settings:
	omp_set_num_threads(1);

	double tstart = omp_get_wtime();

	//Will be either 101 (e = 10) or 201 (e = 5)
	int bucketCount = (int)ceil((1000 / epsilon)) + 1;

	struct pointNode **bucketList = calloc(pow(bucketCount, 2), sizeof(void *));

	//Populate buckets with pointNode objects in linked lists
	for (int i = 0; i < N; i++)
	{
		double col = data[i].x;
		double row = data[i].y;
		int xBucketVal = col / epsilon;
		int yBucketVal = (bucketCount - (row / epsilon));
		int bucketIndex = yBucketVal * bucketCount + xBucketVal;

		struct pointNode **nodeToAdd = &(bucketList[bucketIndex]);

		//Loop until we find a spot to place our node in the bucket
		for (; *nodeToAdd; nodeToAdd = &((*nodeToAdd)->next))
		{
		}
		*nodeToAdd = malloc(sizeof(struct pointNode));
		(*nodeToAdd)->point = data[i];
		(*nodeToAdd)->next = NULL;
	}

	//Init vars for omp loop
	int i;
	struct pointNode *pointList;
	int row, col;

//This loop and its conditions do index math to check each bin and its relevant neighbors in 1D space
#pragma omp parallel for private(i, pointList, row, col) shared(bucketCount) reduction(+ \
																					   : count)
	for (int i = 0; i < bucketCount * bucketCount; ++i)
	{
		pointList = bucketList[i];
		//For each bucket, check if it is empty
		if (!pointList)
			continue;

		//If it is not empty, get its row and col vals
		const int row = i / bucketCount;
		const int col = i - row * bucketCount;
		//Each point in bucket
		for (; pointList; pointList = pointList->next)
		{
			count += 1;
			//Check points within our own bucket
			if (pointList->next)
			{
				count += checkBucket(pointList->point, pointList->next, epsilon);
			}

			//If the entry to the right is valid, and not null
			if (col + 1 != bucketCount && bucketList[i + 1])
			{
				count += checkBucket(pointList->point, bucketList[i + 1], epsilon);
			}

			//If the entry below is valid, and not null
			if (row + 1 != bucketCount && bucketList[i + bucketCount])
			{
				count += checkBucket(pointList->point, bucketList[i + bucketCount], epsilon);
			}
			//If the entry diagonal is valid, and not null
			if (row + 1 != bucketCount && col + 1 != bucketCount && bucketList[i + bucketCount + 1])
			{
				count += checkBucket(pointList->point, bucketList[i + bucketCount + 1], epsilon);
			}
			//If the entry up and to the right is valid, and not null
			if (row - 1 != -1 && col + 1 != bucketCount && bucketList[i - bucketCount + 1])
			{
				count += checkBucket(pointList->point, bucketList[i - bucketCount + 1], epsilon);
			}
		}
	}
	double tend = omp_get_wtime();

	printf("Count: %lld", count);
	printf("\nTotal time (s): %f", tend - tstart);

	free(data);
	free(bucketList);
	printf("\n");
	return 0;
}

int checkBucket(const struct pointData currentPoint, struct pointNode *pointsToCheck, const double epsilon)
{
	int count = 0;
	//Each point in bucket, this is slower with parallelism due to thread overheads!
	//#pragma omp parallel for private(currentPoint)
	for (; pointsToCheck; pointsToCheck = pointsToCheck->next)
	{
		if (computeEuclidDist(currentPoint, pointsToCheck->point) <= epsilon)
		{
			count += 2;
		}
	}

	return count;
}

double computeEuclidDist(struct pointData p1, struct pointData p2)
{
	return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
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
