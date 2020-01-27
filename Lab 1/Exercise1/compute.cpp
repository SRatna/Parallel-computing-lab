#include <iostream>
#include <iomanip>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

extern double getTime();
extern double f1();
extern double f2(double a);
extern double f3(double a);
extern double f4(double a);
extern double f5(double a);
extern double f6(double a, double b);
extern double f7(double a);

void *runF2(void *value)
{   
    double v = *(double *) value;
    *(double *) value = f2(v);
}
void *runF3(void *value)
{   
    double v = *(double *) value;
    *(double *) value = f3(v);
}
void *runF4(void *value)
{   
    double v = *(double *) value;
    *(double *) value = f4(v);
}
void *runF5(void *value)
{   
    double v = *(double *) value;
    *(double *) value = f5(v);
}
void *runF7(void *value)
{   
    double v = *(double *) value;
    *(double *) value = f7(v);
}
void *runF6(void *value)
{   
    double v1 = *(double *) value;
    double v2 = *(((double *) value) + 1) ;
    *(double *) value = f6(v1, v2);
}
int main(int argc, char **argv)
{
	double a, b, c, d, e, f, res;
    double g[2];
	double start = getTime();
	a = f1();
    pthread_t f2id, f3id, f4id, f5id, f6id, f7id;
    b = a;
    c = a;
    d = a;
    pthread_create(&f2id, NULL, runF2, &b);
    pthread_create(&f3id, NULL, runF3, &c);
    pthread_create(&f4id, NULL, runF4, &d);
    pthread_join(f2id, NULL);
    pthread_join(f3id, NULL);
    pthread_join(f4id, NULL);
    e = b;
    f = d;
    g[0] = b;
    g[1] = c;
    pthread_create(&f5id, NULL, runF5, &e);
    pthread_create(&f7id, NULL, runF7, &f);
    pthread_create(&f6id, NULL, runF6, g);
    pthread_join(f5id, NULL);
    pthread_join(f6id, NULL);
    pthread_join(f7id, NULL);
	res = e + f + g[0];
	cout << "Result = " << setprecision(18) << res << "\n";
	cout << "Time: " << (getTime()-start) << "\n";
	return 0;
}
