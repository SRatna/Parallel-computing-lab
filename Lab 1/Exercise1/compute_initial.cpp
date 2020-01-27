#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>

using namespace std;


extern double getTime();
extern double f1();
extern double f2(double a);
extern double f3(double a);
extern double f4(double a);
extern double f5(double a);
extern double f6(double a, double b);
extern double f7(double a);

int main(int argc, char **argv)
{
	double a, b, c, res;
	double start = getTime();

	a = f1();
    b = f2(a);
	c = f3(a);
	a = f4(a);
	res = f5(b) + f7(a) + f6(b,c);
	
	cout << "Result = " << setprecision(18) << res << "\n";
	cout << "Time: " << (getTime()-start) << "\n";
	return 0;
}