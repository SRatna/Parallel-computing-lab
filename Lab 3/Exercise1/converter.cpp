#include <stdlib.h>

#include "converter.h"


unsigned int  Converter::maxN;             // Number of fields
unsigned int  Converter::maxK;             // Number of boxes
unsigned long ** Converter::cacheNoverK;   // cacheNoverK[n][k] contains (n+1) over (k+1)
unsigned long *** Converter::cacheConfNo;  // cacheConfNo[n][k][s] contains the number
                                           // of the first configuration where the first
                                           // box is on field 's'.

// Initialize the array cacheNoverK.
void Converter::initNoverK()
{
	// (n k) = n*(n-1)*...*(n-k+1) / k*(k-1)*...*1
	// (n k) = (n-1 k) * n / (n-k)
	// (n k) = (n k-1) * (n-k+1) / k
	for (unsigned int n=1; n<=maxN; n++) {
		cacheNoverK[n-1][0] = n;
		for (unsigned int k=2; k<=maxK; k++) {
			cacheNoverK[n-1][k-1] = cacheNoverK[n-1][k-2] * (n-k+1) / k;
		}
	}
}

// Returns the value of the binomial coefficient 'n over k' (n k).
unsigned long Converter::nOverK(unsigned int n, unsigned int k)
{
	return k == 0 ? 1 : cacheNoverK[n-1][k-1];
}

// Initialize the array cacheConfNo.
void Converter::initConfNo()
{
	for (unsigned int n=1; n<maxN; n++) {
		for (unsigned int k=0; k<=n && k<maxK; k++) {
			cacheConfNo[n][k][0] = 0;
			for (unsigned int i=1; i<=n-k; i++) {
				cacheConfNo[n][k][i] = cacheConfNo[n][k][i-1] + nOverK(n-i+1,k);
			}
		}
	}
}

// Return the number of the first configuration where the first
// box is on field 's'.
unsigned long Converter::confNo(unsigned int n, unsigned int k, unsigned int s)
{
	return cacheConfNo[n-1][k-1][s];
}

// Find the largest entry <= 'no' in the array 'ary[lo:hi-1]' and return its index.
unsigned int Converter::find(unsigned long ary[], unsigned int lo, unsigned int hi,
							 unsigned long no)
{
	while (lo < hi) {
		int m = (lo+hi+1)/2;
		if (ary[m] == no)
			return m;
		if (ary[m] < no) {
			lo = m;
		}
		else {
			hi = m-1;
		}
	}
	return lo;
}

// In a situation with 'n' remaining fields and 'k' remaining boxes for a configuration
// number 'no', search the position 's' of the first remaining box and the number of the
// first configuration, where this box is on field 's'.
unsigned int Converter::findPos(unsigned int n, unsigned int k, unsigned long no, unsigned long* val)
{
	unsigned long * ary = cacheConfNo[n-1][k-1];
	unsigned int pos = find(ary, 0, n-k, no);
	*val = ary[pos];
	return pos;
}

// =========================================================
	
/** Initialize the class. Arguments:
 *   n = number of fields
 *   k = number of boxes
 */
void Converter::init(unsigned int n, unsigned int k)
{
	maxN = n;
	maxK = k;
	
	cacheNoverK = new unsigned long*[n];
	for (unsigned int i=0; i<n; i++)
		cacheNoverK[i] = new unsigned long[k]();
	initNoverK();
	
	cacheConfNo = new unsigned long**[n];
	for (unsigned int i=0; i<n; i++) {
		cacheConfNo[i] = new unsigned long*[k];
		for (unsigned int j=0; j<k; j++)
			cacheConfNo[i][j] = new unsigned long[n]();
	}
	initConfNo();
}

/** Return the number of possible box configurations. */
unsigned long Converter::getNumConfigs()
{
	return nOverK(maxN, maxK);
}

/** Determine the configuration number from the box positions in 'boxpos'. */
unsigned long Converter::configToNo(unsigned int boxpos[])
{
	unsigned long no = 0;
	unsigned int startpos = 0;
	for (unsigned int i=0; i<maxK; i++) {
		no += confNo(maxN-startpos, maxK-i, boxpos[i]-startpos);
		startpos = boxpos[i] + 1;
	}
	return no;
}

/** Determine the box positions corresponding to the specified configuration number. */
void Converter::noToConfig(unsigned long no, unsigned int * boxpos)
{
	unsigned int startpos = 0;
	for (unsigned int i=0; i<maxK; i++) {
		unsigned long val;
		unsigned int pos = findPos(maxN-startpos, maxK-i, no, &val);
		boxpos[i] = startpos + pos;
		no -= val;
		startpos = boxpos[i] + 1;
	}
}
