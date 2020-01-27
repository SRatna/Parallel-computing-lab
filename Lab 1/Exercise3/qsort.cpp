#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#define MINSIZE 2000000

using namespace std;
void *quicksort(void *);
int size;
struct args
{
	int *array;
	int low;
	int high;
};

void *quicksort(void *value)
{
	struct args *v = (struct args *)value;
	int lo = v->low;
	int hi = v->high;
	int *a = v->array;
	int i, j;
	int pivot;
	// cout << "low " << lo << "\n";
	if (lo >= hi)
	{
		return NULL;
	}

	/* Determine pivot (center element of the array) */
	pivot = a[(lo + hi) / 2];
	/* Split aray a into two parts */
	i = lo;
	j = hi;
	while (i <= j)
	{
		while (a[i] < pivot)
			i++;
		while (a[j] > pivot)
			j--;
		if (i <= j)
		{
			int tmp = a[i];
			a[i] = a[j];
			a[j] = tmp;
			i++;
			j--;
		}
	}
	struct args qp;
	qp.array = a;
	qp.low = i;
	qp.high = hi;
	v->low = lo;
	v->high = j;
	if ((hi - lo) > MINSIZE)
	{
		pthread_t t1;
		pthread_create(&t1, NULL, quicksort, &qp);
		quicksort(v);
		pthread_join(t1, NULL);
	}
	else
	{
		quicksort(&qp);
		quicksort(v);
	}
	return NULL;
}

/*
** Auxiliary function: returns the current time in 
** seconds as a floating point number.
*/
double getTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 0.000001;
}

/*
** Initialize an array 'a' of length 'n' with
** random numbers.
*/
void initialize(int *a, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		a[i] = rand();
	}
}

/*
** Check if an array 'a' of length 'n' is sorted.
** If yes, the result is 1, otherwise the result is 0.
*/
int checkSorted(int *a, int n)
{
	int i;

	for (i = 1; i < n; i++)
	{
		if (a[i] < a[i - 1])
			return 0;
	}
	return 1;
}

/*
** Print an array 'a' of length 'n'.
*/
void printArray(int *a, int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		cout << a[i] << ", ";
	}
	cout << "\n";
}

/*
** Main program.
** Invocation: qsort <array length>
*/
int main(int argc, char **argv)
{
	int *array;
	int n;
	double timeStart, timeEnd;

	if (argc < 2)
	{
		cerr << "Usage: qsort <array-size>\n";
		exit(1);
	}

	/*
	** 'n' is the command line parameter,
	** converted into an integer number.
	*/
	n = atoi(argv[1]);
	if ((n <= 0) || (n > 200000000))
	{
		cerr << "Illegal array size!\n";
		exit(2);
	}
	::size = n;
	/*
	** Allocate the array
	*/
	array = new int[n];

	/*
	** Initialisze the array
	*/
	initialize(array, n);

	// printArray(array, n);
	/*
	** Sort and measure the time ...
	*/
	timeStart = getTime();
	struct args qp;
	qp.array = array;
	qp.low = 0;
	qp.high = n - 1;
	quicksort(&qp);
	timeEnd = getTime();

	/*
	** Check it the array is sorted correctly.
	*/
	if (checkSorted(array, n))
	{
		cout << "OK, array is sorted!\n";
	}
	else
	{
		// printArray(array, n);
		cout << "OOPS, array is NOT sorted!\n";
	}

	cout << "Time for sorting: " << setprecision(3) << (timeEnd - timeStart) << " seconds\n";
	return 0;
}
