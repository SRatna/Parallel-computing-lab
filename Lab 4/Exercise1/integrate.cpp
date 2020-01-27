/***************************************************************************
 *  Numerical Integration                                                  *
 ***************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

#define PI 3.1415926535897932384626433832795029

/*
** Globale MPI variable
*/
int myrank, nprocs;


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
** Parallelize this function using an MPI reduction
** operation (MPI_Reduce)!
*/
double Parallel_Integration(int rank, int n, int nprocs)
{
	int i;
	double h, sum, x;

	h = 1.0/(double)n; 
	sum = 0.0;

	for (i=1+rank; i<=n; i+=nprocs) {
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
	return h * sum; 
}

int main(int argc, char *argv[])
{
	int n = 0;
	double local_res, res;
	double t1, t_p, t_s;
	int namelen;
	char name[MPI_MAX_PROCESSOR_NAME];
	
	/* Initialize MPI and set arguments */ 
	MPI_Init(&argc, &argv);

	/* Determine the number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	/* Determine own rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	/* Determine node name */
	MPI_Get_processor_name(name, &namelen);

	/* flush results in the output being printed immediately */
	cout << "Process " << myrank << "/" << nprocs << "started on " << name << "\n" << flush;


	/*
	** Starting from here, you have to parallelize on your own!
	*/  
	if (myrank == 0) {
		cout << "Enter the number of intervals: (0=exit) \n";
		cin >> n;
	}
	t1 = getTime();
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	cout << "Process " << myrank << "/" << nprocs << " n " << n << "\n" << flush;
	local_res = Parallel_Integration(myrank, n, nprocs);
	MPI_Reduce(&local_res, &res, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (myrank == 0) {
		// parallel solution
		cout << "Parallel solution:\n";
		t_p = getTime() - t1;

		cout << "  n: " << n << ", integral is approximatly: " << setprecision(17) << res << "\n"
				<< "  Error is: " << fabs(res-PI) << ", Runtime[sec]: " << setprecision(6) << t_p << "\n";
		
		// serial solution
		cout << "Serial solution:\n";
		t1 = getTime();
		res = Serial_Integration(n);     
		t_s = getTime() - t1;
	
		cout << "  n: " << n << ", integral is approximatly: " << setprecision(17) << res << "\n"
				<< "  Error is: " << fabs(res-PI) << ", Runtime[sec]: " << setprecision(6) << t_s << "\n";

		cout << "  Nprocs: " << nprocs
			<< "  Speedup: " << setprecision(3)
							<< ((t_p > 0) ? t_s/t_p : 0.0) << "\n";
	}
	MPI_Finalize();
	return 0;
}



