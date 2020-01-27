/***************************************************************************
 *  Loop Parallelization
 ***************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

using namespace std;

#define N 1000
#define M 100

extern double a[N], b[N], c[N];
extern double m[M][M];
extern int map[N];

extern void loop1();
extern void loop2();
extern void loop3();
extern void loop4();
extern void loop5();
extern void loop6();
extern void loop7();
extern void loop8();
extern void loop9();
extern void loop10();

double sa[N], sb[N], sc[N];
double sm[M][M];


void init0()
{
	for (int i=0; i<N/2; i++) {
		map[i] = 2*i;
		map[N-1-i] = 2*i;
	}
}

void init1()
{
	for (int i=0; i<N; i++) {
		a[i] = sa[i] = 111.1 + i;
		b[i] = sb[i] = 9999.9 - i;
		c[i] = sc[i] = 2.2 + i/100.0;
	}
}

void init2()
{
	for (int i=0; i<M; i++) {
		for (int j=0; j<M; j++) {
			m[i][j] = sm[i][j] = 432.1 * i + 1.23 * j;
		}
	}
}

void compare1(string loop, string array, double a1[N], double a2[N])
{
	for (int i=0; i<N; i++) {
		if (a1[i] != a2[i]) {
			cout << loop << ": " << array << "[" << i << "] differs!\n";
			return;
		}
	}
}

void compare2(string loop, string array, double a1[M][M], double a2[M][M])
{
	for (int i=0; i<M; i++) {
		for (int j=0; j<M; j++) {
			if (a1[i][j] != a2[i][j]) {
				cout << loop << ": " << array << "[" << i << "][" << j << "] differs!\n";
				return;
			}
		}
	}
}

void check1(string loop)
{
	compare1(loop, "a", a, sa);
	compare1(loop, "b", b, sb);
	compare1(loop, "c", c, sc);
}

void check2(string loop)
{
	compare2(loop, "m", m, sm);
}

void sloop1()
{
	for (int i=0; i<N; i++) {
		sa[i] = sb[i] + sc[0];
		sb[i] = sa[i] - sc[i];
	}
}

void sloop2()
{
	for (int i=1; i<N; i++) {
		sa[i] = sa[i-1];
		sb[i] = sa[i] + sc[i];
	}
}

void sloop3()
{
	for (int i=1; i<N-2; i++) {
		sa[i] = sb[i] + sa[i+2];
		sc[i] = sb[i-1];
	}
}

void sloop4()
{
	for (int i=0; i<N; i++) {
		sa[i] = sa[i] - 0.9 * sa[N/2];
	}
}

void sloop5()
{
	for (int i=0; i<N/2; i++) {
		sa[i+N/3] = (sc[i] - sa[i])/2;
	}
}

void sloop6()
{
	for (int i=0; i<N/3; i++) {
		sa[i+N/3] = (sc[i] - sa[i])/2;
	}
}

void sloop7()
{
	for (int i=0; i<N; i++) {
		sa[map[i]] = sa[i] + sb[i];
	}
}

void sloop8()
{
	for (int i=1; i<M-1; i++) {
		for (int j=1; j<M-1; j++) {
			sm[i][j] = (sm[i-1][j-1] + sm[i-1][j+1] + sm[i+1][j-1] + sm[i+1][j+1]) / 4;
		}
	}
}


int main(int argc, char *argv[])
{
	init1(); sloop1(); loop1(); check1("loop1");
	init1(); sloop2(); loop2(); check1("loop2");
	init1(); sloop3(); loop3(); check1("loop3");
	init1(); sloop4(); loop4(); check1("loop4");
	init1(); sloop5(); loop5(); check1("loop5");
	init1(); sloop6(); loop6(); check1("loop6");
	init0(); init1(); sloop7(); loop7(); check1("loop7");
	init2(); sloop8(); loop8(); check2("loop8");
	return 0;
}



