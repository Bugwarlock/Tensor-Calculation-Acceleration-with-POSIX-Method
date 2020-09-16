
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>
#include <ctime>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

#define N  		12	//	4	8	12	
#define Thread	8	//	2	4	8

int solutions;
int Queens[N];
int numtread_g = 0;
pthread_mutex_t lock;

int put(int Queens[], int row, int column)
{
	int i;
	for (i = 0; i<row; i++) {
		if (Queens[i] == column || abs(Queens[i] - column) == (row - i))
			return -1;
	}
	Queens[row] = column;
	if (row == N - 1) {
		solutions++;
	}
	else {
		for (i = 0; i<N; i++) { // increment row
			put(Queens, row + 1, i);
		}
	}
	return 0;
}


void solve(int Queens[]) {
	int i;
	for (i = 0; i<N; i++) {
		put(Queens, 0, i);
	}
}


int putP(int row, int column)
{
	int i;

	for (i = 0; i<row; i++) {
		if (Queens[i] == column || abs(Queens[i] - column) == (row - i))
			return -1;
	}
	Queens[row] = column;
	if (row == N - 1) {
	//	pthread_mutex_lock(&lock);
		solutions++;
	//	pthread_mutex_unlock(&lock);
	}
	else {
		for (i = 0; i<N; i++) { // increment row
			putP( row + 1, i);
		}
	}
	return 0;
}

void *solveP(void *arg) {
	//	int i;
	int numtread = numtread_g++;
//	int Queens_P[N];

	//	for(i=0; i<N; i++) {
	for (int i = numtread * N / Thread; i < (numtread + 1) * N / Thread; i++) {
				pthread_mutex_lock(&lock);
				putP(0, i);
				pthread_mutex_unlock(&lock);
	}
	return 0;
}


int main()
{

	high_resolution_clock::time_point start_serial = high_resolution_clock::now();
	solve(Queens);
	high_resolution_clock::time_point end_serial = high_resolution_clock::now();
	duration<double> time_span_serial = duration_cast<duration<double>>(end_serial - start_serial);
	cout << "Execution time of Serial code: " << time_span_serial.count() << " seconds" << endl;
	printf("# solutions %d \n", solutions);



	//	parallel
	solutions = 0;
	pthread_t tid[Thread];
	pthread_mutex_init(&lock, NULL);
	high_resolution_clock::time_point start_omp = high_resolution_clock::now();

	for (int i = 0; i < Thread; i++)
		pthread_create(&tid[i], NULL, solveP, (void *)NULL);

	for (int i = 0; i < Thread; i++)
		pthread_join(tid[i], NULL);

	high_resolution_clock::time_point end_omp = high_resolution_clock::now();
	duration<double> time_span_omp = duration_cast<duration<double>>(end_omp - start_omp);
	cout << "Execution time of POSIX code: " << time_span_omp.count() << " seconds" << endl;
	printf("# solutions %d \n", solutions);
	printf("Speedup from Serial to POSIX = %4.2f\n", (float)(time_span_serial.count()) / (float)(time_span_omp.count()));
	pthread_mutex_destroy(&lock);
	getchar();
	return 0;

}
