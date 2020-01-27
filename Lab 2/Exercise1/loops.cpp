/***************************************************************************
 *  Loop Parallelization
 *
 *  Examine the loops below and check, if they can be parallelized.
 *  If so, parallelize these loops. Then run the program with 4 threads.
 *  It checks whether the result is the same as in the sequential version.
 ***************************************************************************/

/* Do not change the following declarations! */
#define N 1000
#define M 100

double a[N], b[N], c[N], a2[N];
double m[M][M];
int map[N];

/* Start here ... */

void loop1()
{
	#pragma omp parallel for
	for (int i=0; i<N; i++) {
		a[i] = b[i] + c[0];
		b[i] = a[i] - c[i];
	}
}

void loop2()
{
	//true dependency
	for (int i=1; i<N; i++) {
		a[i] = a[i-1];
	}
	#pragma omp parallel for
	for (int i=1; i<N; i++) {
		b[i] = a[i] + c[i];
	}
}

void loop3()
{
	//anti dependency
	#pragma omp parallel for
	for (int i=1; i<N; i++) {
		a2[i] = a[i];
	}
	#pragma omp parallel for
	for (int i=1; i<N-2; i++) {
		a[i] = b[i] + a2[i+2];
		c[i] = b[i-1];
	}
}

void loop4()
{
	#pragma omp parallel for
	for (int i=0; i<N/2; i++) {
		a[i] = a[i] - 0.9 * a[N/2];
	}
	a[N/2] = a[N/2] - 0.9 * a[N/2];
	#pragma omp parallel for
	for (int i=(N/2 + 1); i<N; i++) {
		a[i] = a[i] - 0.9 * a[N/2];
	}
}

void loop5()
{
	#pragma omp parallel for
	for (int i=0; i<N/3; i++) {
		a[i+N/3] = (c[i] - a[i])/2;
	}
	#pragma omp parallel for
	for (int i=N/3; i<N/2; i++) {
		a[i+N/3] = (c[i] - a[i])/2;
	}
}

void loop6()
{
	#pragma omp parallel for
	for (int i=0; i<N/3; i++) {
		a[i+N/3] = (c[i] - a[i])/2;
	}
}

void loop7()
{
	/*
	#pragma omp parallel for
	for (int i=1; i<N/2; i+=2) {
		a[map[i]] = a[i] + b[i];
	}
	for (int i=0; i<=N/2; i+=2) {
		a[map[i]] = a[i] + b[i];
	}
	#pragma omp parallel for
	for (int i=(N/2 + 1); i<N; i+=2) {
		a[map[i]] = a[i] + b[i];
	}
	for (int i=(N/2 + 2); i<N; i+=2) {
		a[map[i]] = a[i] + b[i];
	}*/
	for (int i=0; i<N; i++) {
		a[map[i]] = a[i] + b[i];
	}
}

void loop8()
{
	for (int i=1; i<M-1; i++) {
		#pragma omp parallel for
		for (int j=1; j<M-1; j++) {
			m[i][j] = (m[i-1][j-1] + m[i-1][j+1] + m[i+1][j-1] + m[i+1][j+1]) / 4;
		}
	}
}

