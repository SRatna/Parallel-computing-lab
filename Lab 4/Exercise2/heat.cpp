/*************************************************************************
** Computation of heat conduction in a metal plate
** (Iterative solution of a boundary value problem)
** 
** Compile:  g++ -o heat heat.c solver.c
**           g++ -O -o heat heat.c solver.c  // with optimization
** Run:      heat <size> [<epsilon>]
**		        <size>      -- Size of matrix
**	           	<epsilon>   -- accuracy parameter
** Author:   RW
** 
*************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

/*
** Globale MPI variable
*/
int myrank, nprocs;
/*
** The iterative computation terminates, if each element has changed
** by at most 'eps', as compared to the last iteration.
*/
double eps = 0.001;

/*
** Execute the iterative solver on the n*n matrix 'a'.
*/
extern int solver(double **a, int m, int n);
	

/* Auxiliary Functions ************************************************* */

/*
** Allocate a 2D array with m * n elements (m rows with n columns).
** I.e., something like ``new double[m][n]'' ...
*/
double ** New_Matrix(int m, int n)
{
	double **res;
	double *array;
	int i;

	/*
	** Result is a 1D array with pointers to the rows of the 2D array
	*/
	res = new double*[m];

	/*
    ** The actual array is allocated in one piece for efficiency reasons.
	** This corresponds to the memory layout that it would have with static
    ** allocation.
	*/
	array = new double[m*n];

	/*
	** Each row pointer is now initialized with the correct value.
	** (Row i starts at element i*n)
	*/
	for (i = 0; i < m; i++) {
		res[i] = &array[i * n];
	}

	return res;
}

/*
** Deallocate a 2D array.
*/
void Delete_Matrix(double **matrix)
{
	delete[] matrix[0];
	delete[] matrix;
}

/*
** Auxiliary function: Print an element of a 2D array.
void print(double **a, int x, int y)
{
	cout << "  a[" << setw(4) << x << "][" << setw(4) << y << "] = "
		 << setw(0) << setprecision(18) << a[x][y] << "\n";
}
*/

/*
** Returns the current time in seconds as a floating point value
*/
double getTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 0.000001;
}

/* File output ******************************************************** */

/*
** Write the n*n matrix 'a' into the file 'Matrix.txt'.
*/
void Write_Matrix(double **a, int m, int n, int p)
{
	int i, j;
	int start_i = p != 0 ? 1 : 0;
	int end_i = p != nprocs - 1 ? m - 1 : m;
	/* Open file for writing */
	fstream file("Matrix.txt", ios::out|ios::trunc);
	
	if (!file.is_open()) {
		cerr << "Cannot open file 'Matrix.txt' for writing!";
		exit(1);
	}
	if (p == 0) {
		/* Write the size of the matrix */
		file << n << "\n\n";
	}
	/* Write the matrix elements into the file, row by row */
	file << setprecision(10);
	for (i = start_i; i < end_i; i++) {
		for (j = 0; j < n; j++) {
			file << a[i][j] << "\n";
		}
		file << "\n";
	}

	/* Close file */
	file.close();
}

int size(int n, int p) {
	return ((n + p) / nprocs);
}
int max(int m, int n) {
	return m > n ? m : n;
}
int start_index(int n, int p) {
	// n ÷ np · p + max(p − (np − n mod np), 0)
	return (n / nprocs) * p + max((p - (nprocs - (n % nprocs))), 0);
}
void print( double **a, int x, int y, int n, int p) {
	// ((x-start >= 0) && (x-start < size))
	int x_start = x - start_index(n, p);
	x_start = p != 0 ? x_start + 1 : x_start;
	if (x_start >= 0 && x_start < size(n, p)) {
		cout << "  a[" << setw(4) << x << "][" << setw(4) << y << "] = "
		 << setw(0) << setprecision(18) << a[x_start][y] << "\n";
	}
}
/* *********************************************************************** */

int main(int argc, char **argv)
{
	int i, j;
	int m, n;
	double **a, **b;
	double start, end;
	int namelen;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;

	/* Initialize MPI and set arguments */ 
	MPI_Init(&argc, &argv);

	/* Determine the number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	/* Determine own rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	/* Determine node name */
	MPI_Get_processor_name(name, &namelen);

	/* flush results in the output being printed immediately */
	// cout << "Process " << myrank << "/" << nprocs << "started on " << name << "\n" << flush;
	if (myrank == 0) {
		if ((argc < 2) || (argc > 3)) {
			cerr << "Usage: heat <size> [<epsilon>] !\n\n"
				<< "   <size>      -- Size of matrix\n"
				<< "   <epsilon>   -- accuracy parameter\n";
			MPI_Abort(MPI_COMM_WORLD, 0);
		}

		/*
		** First argument: size of the matrix
		*/
		n = atoi(argv[1]);
		if ((n < 3) || (n > 6000)) {
			cerr << "Error: size out of range [3 .. 6000] !\n";
			MPI_Abort(MPI_COMM_WORLD, 0);
		}

		/*
		** Second (optional) argument: "accuracy parameter" eps
		*/
		if (argc >= 3) {
			eps = atof(argv[2]);
		}
		if (eps <= 0) {
			cerr <<	"Error: epsilon must be > 0! "
				<<	"(try values between 0.01 and 0.0000000001)\n";
			MPI_Abort(MPI_COMM_WORLD, 0);
		}
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (argc >= 3) {
		MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	// calculate size: ghost cells considered
	m = size(n, myrank);
	m = (myrank == 0 || myrank == (nprocs - 1)) ? m + 1 : m + 2;
	// cout << "Process " << myrank << "/" << nprocs << " size " << m << "\n" << flush;
	// allocate and initialize new array for each process (m by n)
	b = New_Matrix(m, n);
	if (b == NULL) {
		cerr << "Can't allocate matrix !\n";
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			b[i][j] = 0;
		}
	}
	int niter;
	if (myrank == 0) {
		a = New_Matrix(n, n);
		if (a == NULL) {
			cerr << "Can't allocate matrix !\n";
			MPI_Abort(MPI_COMM_WORLD, 0);
		}
		for (i=0; i<n; i++) {
			for (j=0; j<n; j++) {
				a[i][j] = 0;
			}
		}
		/*
		** Assign the boundary values:
		** The upper left and the lower right corner are cold (value 0),
		** the lower left and the upper right corner are hot (value 1),
		** between the corners, the temperature is changing linearly.
		*/
		for (i=0; i<n; i++) {
			double x = (double)i / (n-1);
			a[i][0]       = x;
			a[n-1-i][n-1] = x;
			a[0][i]       = x;
			a[n-1][n-1-i] = x;
		}
		start = getTime();
		// copy into its own array
		for (i=0; i<m-1; i++) {
			for (j=0; j<n; j++) {
				b[i][j] = a[i][j];
			}
		}
		// send to other processes
		for (int proc=1; proc<nprocs; proc++) {
			int s_i = start_index(n, proc);
			// cout << "p" << s_i << "\n";
			MPI_Send(a[s_i], size(n, proc) * n, MPI_DOUBLE, proc, 0, MPI_COMM_WORLD);
		}
	} else {
		// receive from 0th process and keep in b array
		MPI_Recv(b[1], size(n, myrank) * n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
	}
	niter = solver(b, m, n);
	end = getTime();
	// cout << "Result: " << niter << " process: " << myrank << " iterations\n";

	/*
	** Write the matrix into a file
	*/

	if (n <= 1000) {
		Write_Matrix(b, m, n, myrank);
	}

	/*
	** Statistics and some verification values
	cout << "Result: " << niter << " iterations\n";
	i = n/8;
	*/
	i = n/8;

	print(b, n-1-i, i, n, myrank);
	print(b, n-1-i, i/2, n, myrank);
	print(b, n/2, n/2, n, myrank);
	print(b, i/2, n-1-i, n, myrank);
	print(b, i, n-1-i, n, myrank);
	if (myrank == 0) {
		cout << "Result: " << niter << " iterations\n";
		double time = (end-start);
		cout << fixed << setprecision(3) << "Runtime: " << time << " s\n";
		cout << "Performance: " << (1e-9*niter*(n-2)*(n-2)*4/time) << " GFlop/s\n";
	}

	MPI_Finalize();

	return 0;
}

