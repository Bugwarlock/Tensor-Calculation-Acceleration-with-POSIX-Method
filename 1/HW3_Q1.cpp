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
//#define Array_Size 100000
#define Array_Size 1000000

float *a = new  float[Array_Size];

//Parallel
void *task(void *arg)
{
	int *incoming_param = (int *)arg;
	float *result = (float *)malloc(sizeof(float));
	int i;
	for (i = *incoming_param * (Array_Size / NUM_THREADS); i<(*incoming_param +1) * (Array_Size / NUM_THREADS); i ++)
		if (*(a + i ) > *result)
			*result = *(a + i );

	return result;
}
int main(void)
{
	for (int i = 0; i < Array_Size; i++)
		a[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0));

	high_resolution_clock::time_point start_serial = high_resolution_clock::now();



	int i;
	float max = 0.0;
	for (i = 0; i<Array_Size; i++) {
		if (a[i] >= max)
			max = a[i];
	}
	high_resolution_clock::time_point end_serial = high_resolution_clock::now();
	duration<double> time_span_serial = duration_cast<duration<double>>(end_serial - start_serial);
	cout << "Execution time of Serial code: " << time_span_serial.count() << " seconds" << endl;
	printf("%f,\n", max);


	pthread_t tid[NUM_THREADS];
	int in_param[NUM_THREADS] = { 0, 1,2,3};
	float maxt = 0.0;
	void *thread_result;
	float *return_value;
	high_resolution_clock::time_point start_omp = high_resolution_clock::now();


	for (int i = 0; i < NUM_THREADS; i++)
		pthread_create(&tid[i], NULL, task, (void *)&in_param[i]);
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(tid[i], &thread_result);
		return_value = (float *)thread_result;
		if (*return_value > maxt)
			maxt = *return_value;
	}

	high_resolution_clock::time_point end_omp = high_resolution_clock::now();
	duration<double> time_span_omp = duration_cast<duration<double>>(end_omp - start_omp);
	cout << "Execution time of POSIX code: " << time_span_omp.count() << " seconds" << endl;

	printf("%f ,\n", maxt);
	printf("Speedup from Serial to POSIX = %4.2f\n", (float)(time_span_serial.count()) / (float)(time_span_omp.count()));
	free(return_value);
	//end of Parallel
	getchar();
	return 0;
}