#include <iostream>
#include <math.h>
#include <sys/time.h>

using namespace std;

/* ==================================================
   Thus file should / must not be changed !!!
   ================================================== */

/*
** Auxiliary function: returns the current time in seconds as a floating point value
*/
double getTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 0.000001;
}

/*
** Auxiliary function: calculates for a given time
*/
static void doCompute(double computeTime)
{
	volatile static double glob = 0;
	int i;
	double start = getTime();
	while (getTime() - start < computeTime) {
		for (i=0; i<100000; i++)
			glob += sin(i/10000.0);
	}
}

/*
** Compute intensive functions ...
*/
double f1()
{
	cout << "f1 started\n";
	doCompute(1.0);
	cout << "f1 ended\n";
	return 0.13456;
}

double f2(double a)
{
	cout << "f2 started\n";
	doCompute(1.0);
	cout << "f2 ended\n";
	return a*1.1;
}

double f3(double a)
{
	cout << "f3 started\n";
	doCompute(1.0);
	cout << "f3 ended\n";
	return a+7.65;
}

double f4(double a)
{
	cout << "f4 started\n";
	doCompute(1.0);
	cout << "f4 ended\n";
	return 999*a;
}

double f5(double a)
{
	cout << "f5 started\n";
	doCompute(1.0);
	cout << "f5 ended " << "\n";
	return 1e20*a/0.148016;
}

double f6(double a, double b)
{
	cout << "f6 started\n";
	doCompute(1.0);
	cout << "f6 ended\n";
	return a-1.1+b;
}

double f7(double a)
{
	cout << "f7 started\n";
	doCompute(1.0);
	cout << "f7 ended\n";
	return -1e20+a*1.234e8;
}

