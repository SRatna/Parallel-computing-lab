#include <string>
#include <iostream>

#include "dfsdepthmap.h"

using namespace std;


/**
 * For depth first search, this class performs a mapping from a configuration number to the
 * lowest depth found so far for the corresponding configuration (i.e., the minimum number of
 * moves from the initial configuration to the given configuration, which has been found until
 * now).
 */


/**
 * Constructor: Creates a new mapping for configuration numbers between
 * 0 and numConf-1 and a maximum depth of 'maxDepth'.
 */
DFSDepthMap::DFSDepthMap(unsigned long numConf, unsigned int maxDepth)
{
	depth_length = index1(numConf-1) + 1;
	depth = new volatile unsigned char*[depth_length]();
	nConfigs = new volatile unsigned int[maxDepth+1]();
}

/**
 *  Destructur: deallocate memory.
 */
DFSDepthMap::~DFSDepthMap()
{
	for (unsigned int i=0; i<depth_length; i++) {
		delete[] depth[i];
	}
	delete[] depth;
}

/**
 * Checks whether there is an entry for 'conf' with a depth <= 'newDepth'.
 * If so, return 'false', else set the depth of 'conf' in the mapping to
 * 'newDepth' and return 'true'.
 */
bool DFSDepthMap::lookup_and_set(unsigned long conf, unsigned int newDepth)
{
	unsigned int i1 = index1(conf);
	unsigned int i2 = index2(conf);

	// If necessary, allocate an array at the second level and initialize it with 0
	if (depth[i1] == NULL)
		depth[i1] = new unsigned char[BLOCKSIZE]();
	
	// If there is an entry with equal or smaller depth: we are done
	unsigned char old = depth[i1][i2];
	if ((old != 0) && (old <= (unsigned char)newDepth))
		return false;

	// Enter the depth and update the number of configurations for this depth
	depth[i1][i2] = (unsigned char)newDepth;
	if (old != 0)
		nConfigs[old]--;
	nConfigs[newDepth]++;
		
	return true;
}

/**
 * Returns information about RAM and hard disk usage and the number of
 * examined configurations for all depths < 'maxDepth'.
 */
void DFSDepthMap::statistics(unsigned int maxDepth)
{
	for (unsigned int i=1; i<maxDepth; i++)
		cerr << "depth " << i << ": " << nConfigs[i] << "\n";
			 
	unsigned int size = depth_length/1024*sizeof(unsigned int);
	for (unsigned int i=0; i<depth_length; i++) {
		if (depth[i] != NULL)
			size += BLOCKSIZE/1024;
	}
	cout << "Used " << size << " KBytes for arrays\n";
}

