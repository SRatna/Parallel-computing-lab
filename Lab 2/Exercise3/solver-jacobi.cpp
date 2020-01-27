/*************************************************************************
** Iterative solver: Jacobi method
**
** Author:   RW
** 
*************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <math.h>

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
** Execute Jacobi iteration on the n*n matrix 'a'.
*/
int solver(double **a, int n)
{
	int i,j;
	double h;
	double diff;    /* Maximum change since the last iteration */
	int k = 0;      /* Counts iterations (for statistics only ...) */
	double **b = New_Matrix(n,n);  /* Auxiliary matrix for result */

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
		#pragma omp parallel for private(i, j, h)
		for (i=1; i<n-1; i++) {
			for (j=1; j<n-1; j++) {
				b[i][j] = 0.25 * (a[i][j-1] + a[i-1][j]
								  + a[i+1][j] + a[i][j+1]);

				/* Determine the maximum change of the matrix elements */
				h = fabs(a[i][j] - b[i][j]);
				if (h > diff)
					#pragma omp critical
					diff = h;
			}
		}

		/*
		** Copy intermediate result into matrix 'a'
		*/
		#pragma omp parallel for private(i, j)
		for (i=1; i<n-1; i++) {
			for (j=1; j<n-1; j++) {
				a[i][j] = b[i][j];
			}
		}

		k++;
	} while (diff > eps);
	
	Delete_Matrix(b);

	return k;
}

