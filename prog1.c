#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define ARRAYSIZE 1000
#define THREADS 2

void *slave(void *myid);

// shared data
int data[ARRAYSIZE];
int sum=0;
pthread_mutex_t mutex;
int wsize;

void *slave(void *myid)
{
	int i,low,high,myresult=0;

	low = (int) myid * wsize;
	high = low + wsize;

	for (i=low; i<high;i++)
		myresult += data[i];
	
	pthread_mutex_lock(&mutex);
	// add locl sum to global sum
	sum += myresult;
	pthread_mutex_unlock(&mutex);
}

void main(int argc, char *argv[]) {
	struct timeval start,end;
	int i;
	pthread_t tid[THREADS];
	// initialize mutex
	pthread_mutex_init(&mutex,NULL);

	wsize = ARRAYSIZE/THREADS;

	// populate the array with junk data
	for (i=0; i<ARRAYSIZE;i++)
		data[i] = i+1;

	gettimeofday(&start, NULL);
	for (i=0;i<THREADS;i++)
		if (pthread_create(&tid[i],NULL,slave,(void *)i) != 0)
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
