#include <stdlib.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <fstream>

#include "bfsqueue.h"

using namespace std;

/**
 * Data structure for supporting the breadth first search. The data structure primarily implements
 * a queue for configurations, connected with a set (bit set) storing the configurations that have
 * already been visited.
 */


/**
 * Constructor: Create a queue/bit set for configuration numbers between
 * 0 and numConf-1.
 */
BFSQueue::BFSQueue(unsigned long numConf)
	: file("sokoban.tmp", ios::out|ios::in|ios::trunc|ios::binary) // open a temporary file
{
	if (!file.is_open()) {
		cerr << "Cannot open tmp file 'sokoban.tmp'\n";
		exit(1);
	}
	// Delete the file. However, it stays accessible until it is closed.
	// When using the Windows OS, you may need to delete this statement.
	unlink("sokoban.tmp");

	// Allocate arrays and initialize them with NULL. This initialization is caused by the
	// empty pair of parentheses () at the end of the 'new' operator.
	queue_length = qIndex1(numConf-1) + 1;
	queue[0] = new Entry *[queue_length]();
	queue[1] = new Entry *[queue_length]();
	bitset_length = bsIndex1(numConf-1) + 1;
	bitset = new volatile unsigned int*[bitset_length]();
	wrPos = 0;
	rdLength = 0;
	depth = 0;
	file_length = 0;
}

/**
 * Destructur: deallocate memory.
 */
BFSQueue::~BFSQueue()
{
	for (unsigned int i=0; i<queue_length; i++) {
		delete[] queue[0][i];
		delete[] queue[1][i];
	}
	for (unsigned int i=0; i<bitset_length; i++) {
		delete[] bitset[i];
	}
	delete[] queue[0];
	delete[] queue[1];
	delete[] bitset;
	file.close();
}

/**
 * Increase the tree depth by one. The previous write queue becomes the read queue for the
 * new tree depth. The old read queue is stored in a temporary file to determine the solution
 * path at the end.
 */
void BFSQueue::pushDepth()
{
	unsigned int wr = depth % 2;
	unsigned int n1 = qIndex1(wrPos);
	unsigned int n2 = qIndex2(wrPos);

	// Export the old read queue to a file.
	for (unsigned int i=0; i<n1; i++)
		file.write((char *)queue[wr][i], BLOCKSIZE * sizeof(Entry));
	if (n2 > 0)
		file.write((char *)queue[wr][n1], n2 * sizeof(Entry));
	file_length += ((unsigned long)BLOCKSIZE) * n1 + n2;
	
	depth++;
	rdLength = wrPos;
	wrPos = 0;
}

/**
 * Checks if the given configuration is already contained in the bit set. If not, the
 * configuration is entered in the bit set and the configuration, the index of the predecessor
 * configuration and the number of the moved box are added to the write queue.
 */
bool BFSQueue::lookup_and_add(unsigned long conf, unsigned int predIndex, unsigned int box)
{
	unsigned int bitmask = 1 << bsBitPos(conf);
	unsigned int i1 = bsIndex1(conf);
	unsigned int i2 = bsIndex2(conf);
	
	// If necessary, allocate an array at the second level and initialize it with 0
	if (bitset[i1] == NULL)
		bitset[i1] = new unsigned int[BLOCKSIZE]();

	// If the configuration is in the bit set: we are done
	if ((bitset[i1][i2] & bitmask) != 0)
		return false;
	// if(__sync_fetch_and_and(bitset[i1][i2], bitmask) != 0)
	// 	return false;
	// add the configuration to the bit set
	bitset[i1][i2] |= bitmask;

	// Append the configuration, the index of the predecessor configuration and the
	// number of the moved box at the end of the write queue.
	
	unsigned int wr = depth % 2;
	unsigned int n1 = qIndex1(wrPos);
	unsigned int n2 = qIndex2(wrPos);

	// If necessary, allocate an array at the second level and initialize it
	if (queue[wr][n1] == NULL)
		queue[wr][n1] = new Entry[BLOCKSIZE]();

	// Write the new entry at position wrPos into the write queue
	queue[wr][n1][n2].set(conf, wrPos + (rdLength - predIndex), box);

	// Increment the write position
	wrPos++;
	
	return true;
}

/**
 * Return the length of the read queue.
 */
unsigned int BFSQueue::length()
{
	return rdLength;
}

/**
 * Return the i-th entry in the read queue (configuration as return value;
 * moved box in *box).
 */
unsigned long BFSQueue::get(unsigned int i, unsigned int * box)
{
	unsigned int rd = (depth-1) % 2;
	Entry *e = &queue[rd][qIndex1(i)][qIndex2(i)];
	if (box != NULL)
		*box = e->box;
	return e->config;
}

/**
 * Return the solution path as an array of configurations. The parameter conf is the
 * solution configuration, predIndex the index of the predecessor configuration. In *path_length
 * the length of the path is returned. The result is allocated dynamically and should be 
 * deallocated using delete[].
 */
unsigned long * BFSQueue::getPath(unsigned long conf, unsigned int predIndex,
								  unsigned int * path_length)
{
	unsigned long * path = new unsigned long[depth+1];
	path[depth] = conf;
	unsigned int pos = rdLength - predIndex - 1;
	long abspos = file_length * sizeof(Entry); // XX
	
	// Iterate the path in reversed order
	for (int k = depth-1; k>=0; k--) {
		Entry e;
		// Search the entry for the predecessor configuration in the file and load it
		file.seekg(-((int)pos+1)*sizeof(Entry), ios::cur);
		file.read((char *)&e, sizeof(Entry));
		path[k] = e.config;
		pos = e.pred;
	}
	*path_length = depth+1;
	return path;
}

/**
 * Returns information about RAM and hard disk usage.
 */
void BFSQueue::statistics()
{
	unsigned int size = 2*queue_length*sizeof(Entry *)/1024;
	for (unsigned int i=0; i<queue_length; i++) {
		if (queue[0][i] != NULL)
			size += BLOCKSIZE/1024*sizeof(Entry);
		if (queue[1][i] != NULL)
			size += BLOCKSIZE/1024*sizeof(Entry);
	}
	cout << "Used " << size << " KBytes for arrays\n";
	
	size = bitset_length*sizeof(unsigned int *)/1024;
	for (unsigned int i=0; i<bitset_length; i++) {
		if (bitset[i] != NULL)
			size += BLOCKSIZE/1024*sizeof(unsigned int);
	}
	cout << "Used " << size << " KBytes for bit set\n";
	
	size = file_length*sizeof(Entry)/1024;
	cout << "Used " << size << " KBytes for temp file\n";
}

