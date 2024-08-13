#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include "linebreaker.h"

#define ARRAYSIZE 1000
// #define THREADS 2

void *slave(void *myid);

// shared data
int data[ARRAYSIZE];
int sum=0;
pthread_mutex_t mutex;
int wsize;

void *slave(void *myid)
{
	int i,low,high,myresult=0;

	low = (int)(__intptr_t) myid * wsize;
	high = low + wsize;

	for (i=low; i<high;i++)
		myresult += data[i];
	
	pthread_mutex_lock(&mutex);
	// add locl sum to global sum
	sum += myresult;
	pthread_mutex_unlock(&mutex);
}

void handler(int THREADS) {
	struct timeval start,end;
	int i;
	pthread_t tid[THREADS];

	// initialize sum
	sum = 0;
	// initialize mutex
	pthread_mutex_init(&mutex,NULL);

	wsize = ARRAYSIZE/THREADS;

	// create threads
	gettimeofday(&start, NULL);
	for (i=0;i<THREADS;i++)
		if (pthread_create(&tid[i],NULL,slave,(void *)(__intptr_t)i) != 0)
			perror("Pthread create fails");
	
	// join threads
	for (i=0;i<THREADS;i++)
		if (pthread_join(tid[i],NULL) != 0)
			perror("Pthread_join_fails");
	gettimeofday(&end, NULL);

	long seconds = end.tv_sec - start.tv_sec;
	long microseconds = end.tv_usec - start.tv_usec;
	double elapsed = seconds*1.0 + microseconds*1e-6;

	printf("The sum from 1 to %i is %d\n",ARRAYSIZE,sum);
	printf("Execution time: %.6f seconds\n", elapsed);
}

void main(int argc, char *argv[]) {
	if (argc < 2)
	{
		printf("please provide the max number of threads to consider\n");
		printf("for example:\n");
		printf("\t$>./prog1.o 10\n");
		printf("-will executes the sum of array values with 1 threads to up until 10 threads\n");
		return;
	}

	int to_threads = atoi(argv[1]);

	if (to_threads <= 0)
	{
		printf("negative thread count is not valid: %d\n",to_threads);
	}

	int i;
	// populate the array with junk data
	for (i=0; i<ARRAYSIZE;i++)
		data[i] = i+1;

	nline_break(30);
	for (i=1; i<= to_threads; i++)
	{
		printf("thread count will be used:%d\n",i);
		if (ARRAYSIZE%i == 0)
			handler(i);
		else
			printf("skipping since array size(%d) can't exactly divide by thread count\n",ARRAYSIZE);
		nline_break(30);
	}
}
