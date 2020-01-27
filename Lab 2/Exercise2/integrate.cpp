/***************************************************************************
 *  Numerical Integration                                                  *
 ***************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

using namespace std;


#define PI 3.1415926535897932384626433832795029


/*
** This function is integrated over the interval [0, 1]. The value of the
** integral is exactly PI (for purposes of comparison).
** 
** The function is deliberately written extremely inefficiently in order to
** make the problem more practical: in the case of numerical integration, the
** main effort is usually the calculation of the function, not the summation.
*/
double f(double a) 
{
    double res = 0;
    int i;
    for (i=0; i<1000; i++)
        res += 0.004/(1.0+(a*a));
    return res;
}

/*
** For timing measurements
*/
double getTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 0.000001;
}


/*
** Sequential integration for purposes of comparison.
** Please do not modify this code!
*/
double Serial_Integration(int n)
{
	int i;
	double h, sum, x;
    
	h = 1.0/(double)n; 
	sum = 0.0;
    
	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
    
	return h * sum;
}

/*
** Parallel integration.
**
** Parallelize this function using a reduction!
*/
double Parallel_Integration1(int n)
{
	int i;
	double h, sum, x;
    
	h = 1.0/(double)n; 
	sum = 0.0;
	#pragma omp parallel for reduction(+: sum) private(x)
	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
	
	return h * sum; 
}

/*
** Parallel integration.
**
** Parallelize this function using the OpenMP ordered
** directive, such that the result is exactly the same
** as the result of Serial_Integration()!
*/
double Parallel_Integration2(int n)
{
	int i;
	double h, sum, x;
    
	h = 1.0/(double)n; 
	sum = 0.0;
	#pragma omp parallel for ordered schedule(static, 1) private(x)
	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		x = f(x);
		#pragma omp ordered
		sum += x;
	}
	
	return h * sum;   
}

/*
** Parallel integration.
**
** Parallelize this function, such that the result is exactly
** the same as the result of Serial_Integration()!
** To do so, split the loop into a parallel and a sequential
** part!
**
** Note: In C++, you can create a 'double' array of length n using the
** statement:
**    double *a = new double[n];
** Do not forget to deallocate the array at the end:
**    delete[] a;
*/
double Parallel_Integration3(int n)
{
	int i;
	double h, sum, x;
    double *a = new double[n];
	h = 1.0/(double)n; 
	sum = 0.0;
	#pragma omp parallel for private(x)
	for (i=1; i<=n; i++) {
		x = h * ((double)i - 0.5);
		a[i] = f(x);
	}
	for (i=1; i<=n; i++) {
		sum += a[i];
	}
	delete[] a;
	return h * sum; 
}

/*
** For simplicity: this function performs the integration using
** the function 'integr', returns the runtime and prints the
** obtained speedup, if applicable.
*/
double execute(double (*integr)(int), int n, double t_s)
{
	double t1, t_p;
	double res;
	int nthreads;

	t1 = getTime();
	res = integr(n);     
	t_p = getTime() - t1;
	
    
	cout << "  n: " << n << ", integral is approximatly: " << setprecision(17) << res << "\n"
		 << "  Error is: " << fabs(res-PI) << ", Runtime[sec]: " << setprecision(6) << t_p << "\n";
	if (t_s > 0.0) {
		nthreads = omp_get_max_threads();
		cout << "  Nthreads: " << nthreads
			 << "  Speedup: " << setprecision(3) << (t_s/t_p) << "\n";
	}
	cout << "\n";

	return t_p;
}

int main(int argc, char *argv[])
{
	int n=0;
	double t_s;
	
  
	for (;;) {
		cout << "Enter the number of intervals: (0=exit) \n";
		cin >> n;
		if (n <= 0)
			break;

		/* warm-up ... */
		Serial_Integration(n);
		
		/* serial solution */
		cout << "Serial solution:\n";
		t_s = execute(Serial_Integration, n, 0);
    
		/* parallel solution 1 */
		cout << "Parallel solution 1:\n";
		execute(Parallel_Integration1, n, t_s);
    
		/* parallel solution 2 */
		cout << "Parallel solution 2:\n";
		execute(Parallel_Integration2, n, t_s);
    
		/* parallel solution 3 */
		cout << "Parallel solution 3:\n";
		execute(Parallel_Integration3, n, t_s);
	}
    
	return 0;
}

