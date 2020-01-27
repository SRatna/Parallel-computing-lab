using namespace std;

/**
 * Data structure for supporting the breadth first search. The data structure primarily implements
 * a queue for configurations, connected with a set (bit set) storing the configurations that have
 * already been visited.
 */
class BFSQueue
{
 private:
	/*
	 * Class for an entry in the queue. Each entry contains:
	 * - the number of the configuration
	 * - the relative position of the predecessor configuration in the queue
	 *   (this is needed to determine the solution path when a solution has been found)
	 * - the number of the box that was moved to reach this configuration
	 *   (this is used to preferrably move the same box with the next move)
	 */
	class Entry {
	public:
		unsigned long config;
		unsigned int pred;
		unsigned int box;
		
		inline void set(unsigned long aconfig, unsigned int apred, unsigned int abox) volatile {
			config = aconfig;
			pred = apred;
			box = abox;
		}
	};

	// Split queue. When processing tree depth X
	// - the configurations of depth X-1 which are to be examined will be read from queue[(X-1)%2], and
	// - the successor configurations of depth X will be written into queue[X%2].
	Entry * volatile * queue[2];

	// Number of entries in queue[0] and queue[1], respectively
	unsigned int queue_length;

	// Position of the next free entry in the write queue
	volatile unsigned long wrPos;

	// Length of the read queue
	unsigned int rdLength;

	// Current tree depth
	unsigned int depth;

	// This bit set stores all configurations that already have been examined, in order to
	// avoid (1) cycles and (2) multiple examinations of the same configuration.
	volatile unsigned int * volatile * bitset;

	// Maximum number of entries in the bit set
	unsigned int bitset_length;

	// Swap file. In order to save main memory, only the information for the current tree depth
	// X and the tree depth X-1 are kept in main memory. The entries of the queues for smaller
	// tree depths are exported to a temporary file. When we found a solution, they are needed
	// again to determine the path which lead to the solution.
	fstream         file;

	// Number of entries in the swap file
	unsigned long   file_length;


	// The queues and bit sets are dynamically allocated block by block, and only when
	// necessary. For this purpose a two-level array (i.e., an array of arrays) is used instead
	// of a simple array. The second level is only allocated as required (analogous to
	// two-level page tables in operating systems).
	// The arrays of the second level have 2^16 entries each. An access to a[i] thus is realized
	// as a[i/2^16][i%2^16], where we first check, if a[i/2^16] != NULL. If not, we will allocate
	// the second-level array. For computing the first and second index, we use inline functions.
	// The compiler will copy their code directly to the place where they are used.
	
	static const unsigned int BLOCKBITS = 16;                 // 16 Bit, arrays with 65536 int's
	static const unsigned int BLOCKSIZE = (1<<BLOCKBITS);     // Block size for allocation: 2^16
	static const unsigned int BLOCKMASK = ((1<<BLOCKBITS)-1); // Bit mask where the last 16 bits
	                                                          // are set
	
	inline unsigned int qIndex1(unsigned long i)  { return i >> BLOCKBITS; }
	inline unsigned int qIndex2(unsigned long i)  { return i & BLOCKMASK; }

	// The bit set is implemented as a two-level array of 32-bit values. For a given configuration
	// number, the function bsBitPos returns the bit position within an array element.
	
	static const unsigned int WORDBITS = 5;                 // 5 Bit = 0..31, bits in one int
	static const unsigned int WORDMASK = ((1<<WORDBITS)-1); // Bit mask where the last 5 Bits
	                                                        // are set
	
	inline unsigned int bsIndex1(unsigned long i) { return i >> (WORDBITS + BLOCKBITS); }
	inline unsigned int bsIndex2(unsigned long i) { return (i >> WORDBITS) & BLOCKMASK; }
	inline unsigned int bsBitPos(unsigned long i) { return i & WORDMASK; }

 public:
	/**
	 * Constructor: Create a queue/bit set for configuration numbers between
	 * 0 and numConf-1.
	 */
	BFSQueue(unsigned long numConf);

	/**
	 * Destructur: deallocate memory.
	 */
	~BFSQueue();

	/**
	 * Increase the tree depth by one. The previous write queue becomes the read queue for the
	 * new tree depth. The old read queue is stored in a temporary file to determine the solution
	 * path at the end.
	 */
	void pushDepth();

	/**
	 * Checks if the given configuration is already contained in the bit set. If not, the
	 * configuration is entered in the bit set and the configuration, the index of the predecessor
	 * configuration and the number of the moved box are added to the write queue.
	 */
	bool lookup_and_add(unsigned long conf, unsigned int predIndex, unsigned int box);

	/**
	 * Return the length of the read queue.
	 */
	unsigned int length();
	
	/**
	 * Return the i-th entry in the read queue (configuration as return value;
	 * moved box in *box).
	 */
	unsigned long get(unsigned int i, unsigned int * box);

	/**
	 * Return the solution path as an array of configurations. The parameter conf is the
	 * solution configuration, predIndex the index of the predecessor configuration. In *path_length
	 * the length of the path is returned. The result is allocated dynamically and should be 
	 * deallocated using delete[].
	 */
	unsigned long * getPath(unsigned long conf, unsigned int predIndex, unsigned int * path_length);

	/**
	 * Returns information about RAM and hard disk usage.
	 */
	void statistics();
};

