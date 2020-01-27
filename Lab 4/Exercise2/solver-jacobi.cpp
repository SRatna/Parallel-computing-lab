/*************************************************************************
** Iterative solver: Jacobi method
**
** Author:   RW
** 
*************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

using namespace std;


/*
** The iterative computation terminates, if each element has changed
** by at most 'eps', as compared to the last iteration.
*/
extern double eps;


/* Auxiliary Functions *************************************************** */

extern double ** New_Matrix(int m, int n);
extern void Delete_Matrix(double **matrix);


/* Jacobi iteration ***************************************************** */

/*
** Execute Jacobi iteration on the m*n matrix 'a'.
*/
int solver(double **a, int m, int n)
{
	int i,j;
	double h;
	double diff, gdiff;    /* Maximum change since the last iteration */
	int k = 0;      /* Counts iterations (for statistics only ...) */
	double **b = New_Matrix(m,n);  /* Auxiliary matrix for result */
	MPI_Status status;
	int nprocs, myrank;
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	if (b == NULL) {
		cerr << "Jacobi: Can't allocate matrix\n";
		exit(1);
	}
	
	/*
	** Iterate until convergence is achieved. Here: until the maximum
	** change of a matrix element is smaller or equal than 'eps'.
	*/
	do {
		diff = 0;
		for (i=1; i<m-1; i++) {
			for (j=1; j<n-1; j++) {
				b[i][j] = 0.25 * (a[i][j-1] + a[i-1][j] + a[i+1][j] + a[i][j+1]);
				/* Determine the maximum change of the matrix elements */
				h = fabs(a[i][j] - b[i][j]);
				if (h > diff)
					diff = h;
			}
		}
		/*
		** Copy intermediate result into matrix 'a'
		*/
		for (i=1; i<m-1; i++) {
			for (j=1; j<n-1; j++) {
				a[i][j] = b[i][j];
			}
		}
		k++;
		// ghost cells fill up steps
		// Step 1: Send downwards, recieve from above
		if (myrank != nprocs-1)
			MPI_Send(a[m-2], n, MPI_DOUBLE, myrank+1, 0, MPI_COMM_WORLD);
		if (myrank != 0)
			MPI_Recv(a[0], n, MPI_DOUBLE, myrank-1, 0, MPI_COMM_WORLD, &status);
		// Step 2: Send upwards, recieve from below
		if (myrank != 0)
			MPI_Send(a[1], n, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD);
		if (myrank != nprocs-1)
			MPI_Recv(a[m-1], n, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &status);
		
		MPI_Allreduce(&diff, &gdiff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
	} while (gdiff > eps);
	
	Delete_Matrix(b);

	return k;
}

