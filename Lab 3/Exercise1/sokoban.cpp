#include <stdlib.h>
#include <sys/time.h>

#include <string>
#include <iostream>
#include <fstream>

#include "converter.h"
#include "config.h"
#include "bfsqueue.h"
#include "dfsstack.h"
#include "dfsdepthmap.h"

using namespace std;

/**
 * This program solves the game 'Sokoban'. The goal of the game is to push boxes
 * with a player onto target fields (storage locations) in a cartesian grid.
 * In each step only one box can be pushed.
 */


/**
 * Returns the current wall clock time as a floating point number for timing measurements.
 */
static double getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 0.000001;
}

/**
 * Check whether the configuration with number 'succNo' is a successor of
 * the configuration 'conf', and which box must be moved in order to reach
 * this successor configuration.
 */
static unsigned int checkSuccessor(Config *conf, unsigned long succNo)
{
	unsigned int nBoxes = Config::numBoxes(); // Number of boxes
	for (unsigned int box=0; box<nBoxes; box++) {
		for (unsigned int dir=0; dir<4; dir++) {
			if (conf->getNextConfig(box, dir, NULL) == succNo)
				return box;
		}
	}
	cerr << "FATAL ERROR: Invalid solution path!\n";
	cerr << "             This configuration's successor is illegal!\n";
	return -1;
}

/**
 * Print the path for a discovered solution, i.e., the sequence of configurations
 * that leads to the solution.
 */
static void printPath(unsigned long path[], unsigned int length)
{
	if (length > 0) {
		cerr << "\n";
		cerr << "Found solution with " << (length-1) << " pushes\n";
		cout << "\n";
		unsigned int box = -1;
		for (unsigned int i=0; i<length; i++) {
			Config conf(path[i]);
			cout << "Push " << i << ":\n";
			conf.print();
			box = (i < length-1) ? checkSuccessor(&conf, path[i+1]) : -1;
		}
	}
	else {
		cout << "\n";
		cout << "Found NO solution\n";
		cout << "\n";
	}
}

/**
 * Execute a breadth first search from the given starting configuration, in order to find a
 * solution. The search tree is examined layer by layer from top to bottom. For configurations
 * of depth 'depth-1', the possible successor configurations of depth 'depth' are determined
 * and entered into the queue for depth 'depth', if they have not already been examined
 * previously.
 */
static void doBreadthFirstSearch(Config * conf)
{
	// Create the queue for the configurations to be examined.
	// At the beginning, the queue just contains the starting configuration.
	BFSQueue * queue = new BFSQueue(Config::getNumConfigs());
	queue->lookup_and_add(conf->getConfig(), -1, 0);
	queue->pushDepth();
	
	unsigned int nBoxes = Config::numBoxes(); // Number of boxes
	unsigned int depth = 1;                   // Tree depth
	unsigned int length = queue->length();    // Number of configurations at depth 'depth-1'
	unsigned int lastBox;                     // Box that was moved last
	
	// Pass through all layers of the tree with increasing depth until there are no
	// configurations with this depth any more.
	bool solutionFound = false;
	while (length > 0) {
		// Print the progress
		cerr << "depth " << depth << ": " << length << "\n" << flush;
		
		// Consider all configurations of depth 'depth-1'.
		#pragma omp parallel for private(lastBox)
		for (unsigned int i=0; i<length; i++) {
			// Read the configuration from the queue
			Config newConf(queue->get(i, &lastBox));
			// Consider all boxes, starting with the box that was moved last
			for (unsigned int b=0; b<nBoxes; b++) {
				unsigned int box = (b + lastBox) % nBoxes;
				// Consider all directions of movement
				for (unsigned int dir=0; dir<4 && !solutionFound; dir++) {
					unsigned int newBox;
					// Determine the configuration that results from moving box
					// 'box' in direction 'dir'.
					unsigned long c = newConf.getNextConfig(box, dir, &newBox);
					// If the move is valid, check whether the resuling configuration has
					// been examined before. If not, add it to the queue
					if ((c != Config::NONE)) {
						bool configAdded = false;
						#pragma omp critical
						configAdded = queue->lookup_and_add(c, i, newBox);
						// If we found a solution: print it and terminate the search
						if (configAdded && Config::isSolutionConf(c)) {
							solutionFound = true;
							unsigned int len;
							unsigned long * path = queue->getPath(c, i, &len);
							printPath(path, len);
							delete[] path;
							queue->statistics();
						}
					}
				}
			}
		}

		// Advance the queue for the next tree depth
		depth++;
		queue->pushDepth();
		// Number of configurations in the next tree depth
		length = queue->length();
	}

	// If the loop exits normally, there is no solution
	cout << "No solution found!\n";
	queue->statistics();
}

/**
 * Global variable for depth first search
 * - best solution path found so far
 * - length of this path (= depth limit for the search)
 */
static unsigned long * path = NULL;
static unsigned int path_len = 0;

/**
 * Recursive depth first search. If 'conf' is a solution configuration, the
 * recursion is terminated. Otherwise, the procedure is called recursively
 * for all possible successor configurations.
 * The parameter 'lastBox' contains the number of the box that was moved last,
 * so we can preferrably move the same box with the next move. In
 * 'stack' we store the sequence of moves executed so far; 'map' is a mapping 
 * that stores the lowest tree depth found so far for each configuration.
 * It is used to avoid repeated examinations of the same configuration when
 * this is not necessary.
 */
static void recDepthFirstSearch(Config * conf, unsigned int lastBox,
								DFSStack * stack, DFSDepthMap * map)
{
	// Get the configuration number and push it on the stack.
	unsigned long c = conf->getConfig();
	#pragma omp critical
	stack->push(c);
	unsigned int depth = stack->length();
	// If we found a solution: remember the solution path (sequence of moves).
	if (Config::isSolutionConf(c)) {
		delete[] path;
		path = stack->getPath(&path_len);	
		cout << "Found solution: " << (path_len-1) << " pushes\n";
		stack->pop();
		return;
	}

	// If the depth is larger than the length of the best solution path found so far:
	// Terminate the examination of this branch (it cannot contain a better solution
	// any more).
	if (depth >= path_len) {
		stack->pop();
		return;
	}

	// Consider all boxes, starting with the box that was moved last
	unsigned int nBoxes = Config::numBoxes();
	for (unsigned int b=0; b < nBoxes; b++) {
		unsigned int box = (b + lastBox) % nBoxes;
		// Consider all directions of movement
		for (unsigned int dir=0; dir<4; dir++) {
			unsigned int newBox;
			// Determine the configuration that results from moving box
			// 'box' in direction 'dir'.
			c = conf->getNextConfig(box, dir, &newBox);
			
			// If the move is valid, check whether the resuling configuration has
			// already been found at the same or a smaller depth. If not, store
			// the new depth for this configuration.
			if ((c != Config::NONE)) {
				// Recursively continue the search on a copy of the configuration
				bool configAdded = false;
				#pragma omp critical
				configAdded = map->lookup_and_set(c, depth+1);
				if (configAdded) {
					DFSStack *newStack = new DFSStack(*stack);
					#pragma omp task shared(map)
					{
						Config next(c);
						recDepthFirstSearch(&next, box, newStack, map);
						delete newStack;
					}
				}
			}
		}
	}
	stack->pop();
}

/**
 * Wrapper procedure for recursive depth first search. The search starts at the
 * starting configuration 'conf' and continues up to the maximum depth 'maxDepth'.
 */
static void doDepthFirstSearch(Config * conf, unsigned int maxDepth)
{
	DFSStack stack(maxDepth);
	DFSDepthMap map(Config::getNumConfigs(), maxDepth);
	map.lookup_and_set(conf->getConfig(), 1);
	path_len = maxDepth;
	#pragma omp parallel
	#pragma omp single nowait
	recDepthFirstSearch(conf, 0, &stack, &map);

	map.statistics(path_len);

	printPath(path, path_len);
	delete[] path;
}

/**
 * Main program. Invocation:
 *    sokoban <level-file> [<max-depth>]
 * If 'max-depth' is give, a depth first search up to a maximum depth of 'max-depth'
 * is performed, otherwise a breadth first search.
 */
int main(int argc, char **argv)
{
	if ((argc < 2) || (argc > 3)) {
		cerr << "Usage: sokoban <level-file> [<max-depth>]\n";
		exit(1);
	}

	// Initialize the configuration with the starting configuration (level) from the file
	Config * conf = Config::init(argv[1]);

	double ta = getTime();
	if (argc > 2) {
		// depth first search
		unsigned int maxDepth = atoi(argv[2]);
		doDepthFirstSearch(conf, maxDepth+1);
	}
	else {
		// breadth first search
		doBreadthFirstSearch(conf);
	}
	double te = getTime();

	// Print the run time
	cout << "\n";
	cout << "Total time (s): " << (te-ta) << "\n";

	return 0;
}
