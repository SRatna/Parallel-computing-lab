/**
 * This class (with only static attributes and methods) converts a configuration of boxes,
 * i.e., an array containing the positions of the boxes, into an integer (the configuration
 * number), and vice versa.
 */
class Converter
{
 public:
	/** Initialize the class. Arguments:
	 *   n = number of fields
	 *   k = number of boxes
	 */
	static void init(unsigned int n, unsigned int k);
	
	/** Return the number of possible box configurations. */
	static unsigned long getNumConfigs();
	
	/** Determine the configuration number from the box positions in 'boxpos'. */
	static unsigned long configToNo(unsigned int boxpos[]);
	
	/** Determine the box positions corresponding to the specified configuration number. */
	static void noToConfig(unsigned long no, unsigned int * bospos);
	
 private:
	static unsigned int maxN;              // Number of fields
	static unsigned int maxK;              // Number of boxes
	static unsigned long ** cacheNoverK;   // cacheNoverK[n][k] contains (n+1) over (k+1)
	static unsigned long *** cacheConfNo;  // cacheConfNo[n][k][s] contains the number
	                                       // of the first configuration where the first
                                           // box is on field 's'.
	
	// Initialize the array cacheNoverK.
	static void initNoverK();
	
	// Returns the value of the binomial coefficient 'n over k' (n k).
	static unsigned long nOverK(unsigned int n, unsigned int k);
	
	// Initialize the array cacheConfNo.
	static void initConfNo();
	
	// Return the number of the first configuration where the first
	// box is on field 's'.
	static unsigned long confNo(unsigned int n, unsigned int k, unsigned int s);
	
	// Find the largest entry <= 'no' in the array 'ary[lo:hi-1]' and return its index.
	static unsigned int find(unsigned long ary[], unsigned int lo, unsigned int hi,
							 unsigned long no);
	
	// In a situation with 'n' remaining fields and 'k' remaining boxes for a configuration
	// number 'no', search the position 's' of the first remaining box and the number of the
	// first configuration, where this box is on field 's'.
	static unsigned int findPos(unsigned int n, unsigned int k, unsigned long no,
								unsigned long* val);
};
