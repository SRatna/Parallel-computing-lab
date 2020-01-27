/*************************************************************************
** Iterative solver: Gauss/Seidel method
**
** Author:   RW
** 
*************************************************************************/

#include <stdlib.h>
#include <math.h>

/*
** The iterative computation terminates, if the accuracy is at least 'eps'.
*/
extern double eps;

/* Auxiliary Functions *************************************************** */

extern double **New_Matrix(int m, int n);
extern void Delete_Matrix(double **matrix);

/* Gauss/Seidel relaxation *********************************************** */

/*
** Execute Gau�/Seidel relaxation on the n*n matrix 'a'.
*/
int solver(double **a, int n)
{
	/*
	** Simple estimation for the number of iterations, which is needed to
	** achieve the required accuracy.
	*/
	int kmax = (int)(0.35 / eps);
	int i, j, ij;
	int k; /* Counts iterations */

	/*
	** Iterate 'k' times.
	*/
	for (k = 0; k < kmax; k++)
	{
		#pragma omp parallel for private(ij, j, i)
		for (ij = 1; ij < 2 * n - 4; ij++)
		{
			int ja = (ij <= n - 2) ? 1 : ij - (n - 3);
			int je = (ij <= n - 2) ? ij : n - 2;
			for (j = ja; j <= je; j++)
			{
				i = ij - j + 1;
				a[i][j] = 0.25 * (a[i][j - 1] + a[i - 1][j] + a[i + 1][j] + a[i][j + 1]);
			}
		}
	}
	
	return kmax;
}
