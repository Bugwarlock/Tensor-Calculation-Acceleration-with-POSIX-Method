
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include     "intrin.h"
#include     <cstdlib>
#include	 <ctime>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

#define NUM_THREADS 4
#define NUMBER_MAT 1000

// Arrays 
float a[NUMBER_MAT][NUMBER_MAT];
float b[NUMBER_MAT][NUMBER_MAT];
float c[NUMBER_MAT][NUMBER_MAT];

int numtread_g = 0;


//Parallel
void *task(void *arg)
{
//	int *incoming_param = (int *)arg;

	int numtread = numtread_g++;

//	for (i = *incoming_param * (NUMBER_MAT / NUM_THREADS); i<(*incoming_param + 1) * (NUMBER_MAT / NUM_THREADS); i++)
	for (int i = numtread * NUMBER_MAT / NUM_THREADS; i < (numtread + 1) * NUMBER_MAT / NUM_THREADS; i++){
		for (int j = 0; j<NUMBER_MAT; j++) {
			c[i][j] = 0;
			for (int k = 0; k < NUMBER_MAT; k++)
				c[i][j] += a[i][k] * b[j][k];
		}
	}
	return 0;
}
int main(void)
{
	float fTemp;
	// initialize input array
	for (int i = 0; i<NUMBER_MAT; i++) {
		for (int j = 0; j<NUMBER_MAT; j++) {
			a[i][j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10000));
		}
	}

	for (int i = 0; i<NUMBER_MAT; i++) {
		for (int j = 0; j<NUMBER_MAT; j++) {
			b[i][j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 10000));
		}
	}


	//	--------------	Serial	-----------

	//	transpos		
	for (int i = 0; i < NUMBER_MAT; i++)
		for (int j = i + 1; j < NUMBER_MAT; j++) {
			fTemp = b[i][j];
			b[i][j] = b[j][i];
			b[j][i] = fTemp;
		}

	high_resolution_clock::time_point start_serial = high_resolution_clock::now();

	//	computation c
	for (int i = 0; i < NUMBER_MAT; i++) {
		for (int j = 0; j < NUMBER_MAT; j++) {
			c[i][j] = 0;
			for (int k = 0; k < NUMBER_MAT; k++)
				c[i][j] += a[i][k] * b[j][k];
		}
	}

	//	transpos		
	for (int i = 0; i < NUMBER_MAT; i++)
		for (int j = i + 1; j < NUMBER_MAT; j++){
			fTemp = b[i][j];
			b[i][j] = b[j][i];
			b[j][i] = fTemp;
		}



	high_resolution_clock::time_point end_serial = high_resolution_clock::now();
	duration<double> time_span_serial = duration_cast<duration<double>>(end_serial - start_serial);
	cout << "Execution time of Serial code: " << time_span_serial.count() << " seconds" << endl;
	printf("%f,\n", c[10][166]); // verify

	//	---------------		PARALLEL	----------------

//	for (int  i = 0; i<NUMBER_MAT; i++) {
//		for (int j = 0; j<NUMBER_MAT; j++) {
//			c[i][j] = 0.0;
//		}
//	}
	high_resolution_clock::time_point start_omp = high_resolution_clock::now();

	pthread_t tid[NUM_THREADS];
	float maxt = 0.0;

	//	transpos		
	for (int i = 0; i < NUMBER_MAT; i++)
		for (int j = i + 1; j < NUMBER_MAT; j++) {
			fTemp = b[i][j];
			b[i][j] = b[j][i];
			b[j][i] = fTemp;
		}
	//	computation c
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_create(&tid[i], NULL, task, (void *)NULL);

	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(tid[i], NULL);

	//	transpos		
	for (int i = 0; i < NUMBER_MAT; i++)
		for (int j = i + 1; j < NUMBER_MAT; j++) {
			fTemp = b[i][j];
			b[i][j] = b[j][i];
			b[j][i] = fTemp;
		}

	high_resolution_clock::time_point end_omp = high_resolution_clock::now();
	duration<double> time_span_omp = duration_cast<duration<double>>(end_omp - start_omp);
	cout << "Execution time of POSIX code: " << time_span_omp.count() << " seconds" << endl;

	printf("%f,\n", c[10][166]);	// verify
	printf("Speedup from Serial to POSIX = %4.2f\n", (float)(time_span_serial.count()) / (float)(time_span_omp.count()));
//	free(return_value);
	//end of Parallel
	getchar();
	return 0;
}
