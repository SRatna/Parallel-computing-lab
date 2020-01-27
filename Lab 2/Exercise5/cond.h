/*************************************************************************
** Synchronization construct for Gauss/Seidel
**
** Author:   RW
** 
*************************************************************************/

// Only compile, when compiler is invoked with -fopenmp
#ifdef _OPENMP

#include <climits>
#include <unistd.h>

class Cond
{
public:
	/*
	** Constructor. The parameter 'n' specifies the matrix size of the Gauss/Seidel method.
	*/
	Cond(int n)
	{
		last_iter = new int[n]();
		// Row 0 never changes, and therefore is already computed for any 'k'.
		last_iter[0] = INT_MAX;
		//  Row n-1 never changes, and therefore is already computed for any 'k'.
		last_iter[n-1] = INT_MAX;
	}

	/*
	** Destructor.
	*/
	~Cond()
	{
		delete[] last_iter;
	}
		
	/*
	** Signals that the computation of row 'i' in iteration 'k' has been completed.
	*/
	void signal(int k, int i)
	{
		last_iter[i] = k+1;
        #pragma omp flush      // Enforce memory consistency
	}

	/*
	** Wait until the computation of row 'i' in iteration 'k' has been completed.
	*/
	void wait(int k, int i)
	{
        #pragma omp flush      // Enforce memory consistency
		while (last_iter[i] < k+1) {
			usleep(1000);      // Reduces CPU load
            #pragma omp flush  // Enforce memory consistency
		}
	}

private:
    /*
	** last_iter[i] is the last iteration 'k', for which row 'i'
	** has already been computed.
	*/
	int *last_iter;
};
#endif

