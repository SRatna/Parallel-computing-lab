using namespace std;

/**
 * This class implements a stack of configurations for depth first search. It keeps track of
 * the path from the initial configuration to the currently examined configuration.
 */
class DFSStack
{
 private:
	// Stack: array of configuration numbers
	unsigned long * stack;
	// Length of the 'stack' array
	unsigned int stack_length;
	// Stack pointer
	unsigned int sp;

 public:
	/**
	 * Constructor: Creates a stack with maximum depth 'maxDepth'.
	 */
	DFSStack(unsigned int maxDepth);

	/**
	 * Copy constructor: Creates a copy of the given stack.
	 */
	DFSStack(DFSStack &from);

	/**
	 * Destructur: deallocate memory.
	 */
	~DFSStack();

	/**
	 * Pushes the given configuration number onto the stack.
	 */
	void push(unsigned long conf);

	/**
	 * Pops the topmost configuration number from the stack.
	 */
	void pop();

	/**
	 * Return the current depth of the stack.
	 */
	unsigned int length();

	/**
	 * Returns the solution path as an array of configurations. In '*path_length'
	 * the length of this path is returned. The result is allocated dynamically and
	 * should be deallocated using delete[].
	 */
	unsigned long * getPath(unsigned int * path_length);
};
