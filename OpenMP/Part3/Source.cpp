#include "stdafx.h"
#include "omp.h"
#include "intrin.h"
#include "stdio.h"
#include <stdlib.h>

#define 	N 			128 // 128 256 512 1024 a, b, and c are N x N matrices
#define 	NR_THREADS 	2   // 2  4  6  8  10  16


void transpose(int size, float **m)
{
	float fTemp;
	for (int i = 0; i < size; i++)
		for (int j = i + 1; j < size; j++) {
			fTemp = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = fTemp;
		}
}

int main(int argc, char *argv[]) {
	float **m1, **m2, **m3;
	double start_serial, end_serial, start_parallel, end_parallel;

	int	tid, nthreads, i, j, k;
	float fTemp;

	m1 = new float*[N];
	m2 = new float*[N];
	m3 = new float*[N];
	for (i = 0; i < N; i++) {
		m1[i] = new float[N];
		m2[i] = new float[N];
		m3[i] = new float[N];
	}

	printf("Matrix Size is %d\n", N);
	printf("matrix multiplication started\n");
	printf("initializing matrices ...\n\n");
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			m1[i][j] = (float)rand();
			m2[i][j] = (float)rand();
		}
	}

	printf("staring serial version ...\n");
	start_serial = omp_get_wtime();
	transpose(N, m2);
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			float fTemp = 0;
			for (k = 0; k < N; k++)
				fTemp += m1[i][k] * m2[j][k];
			m3[i][j] = fTemp;
		}
	}
	transpose(N, m2);
	end_serial = omp_get_wtime();
	printf("serial run time = %f\n\n", (double)(end_serial - start_serial));

	// openmp version
#pragma omp parallel num_threads(NR_THREADS) shared(m1, m2, m3, nthreads) private (i, j, k, tid)
	{
		tid = omp_get_thread_num();
		if (tid == 0) {  // only thread 0 prints
			nthreads = omp_get_num_threads();
			printf("staring parallel version with %d threads...\n", nthreads);
			start_parallel = omp_get_wtime();  // master thread measures the execution time
		}

		// transpose m2
#pragma omp for
		for (int i = 0; i < N; i++)
			for (int j = i + 1; j < N; j++) {
				fTemp = m2[i][j];
				m2[i][j] = m2[j][i];
				m2[j][i] = fTemp;
			}
		// matrix multiplication
#pragma omp for
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				float fTemp = 0;
				for (k = 0; k < N; k++)
					fTemp += m1[i][k] * m2[j][k];
				m3[i][j] = fTemp;
			}
		}
		// transpose m2
#pragma omp for
		for (int i = 0; i < N; i++)
			for (int j = i + 1; j < N; j++) {
				fTemp = m2[i][j];
				m2[i][j] = m2[j][i];
				m2[j][i] = fTemp;
			}
		if (tid == 0) {	// only thread 0 prints
			end_parallel = omp_get_wtime();
			printf("parallel run time = %f\n\n", (double)(end_parallel - start_parallel));
		}
	} // end of parallel region

	printf("Speedup = %4.2f\n", (float)(end_serial - start_serial) / (float)(end_parallel - start_parallel));
	getchar();
	return 0;
}