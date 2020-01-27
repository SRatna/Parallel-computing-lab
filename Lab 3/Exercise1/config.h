#include "playfield.h"

using namespace std;

/**
 *  This class represents a configuration in the Sokoban game. This includes
 *   - the playing field  (shape, number of fields, positions of the targets, ...)
 *   - the positions of the boxes
 *   - the position of the player
  *  Each configuration may also be represented by a configuration number (i.e., an
 *  integer). This is determined by a configuration number for the box positions and the
 *  position of the player.
 */
class Config
{
 public:
	/**
	 * Special configuration number that allows getNextConfig() to indicate, that there is
	 * no successor configuration.
	 */
	static const unsigned long NONE = -1L;

	/**
	 * Initialization: The file 'fname' contains a string representation of the Sokoban level,
	 * i.e., the initial configuration. The return value is the start configuration.
	 */
	static Config * init(const char * fname);

	/**
	 * Does the specified configuration number represent a solution, i.e., are all boxes on
	 * a target?
	 */
	static bool isSolutionConf(unsigned long conf);

	/**
	 * Returns the maximum amount of configuration numbers. Thus, the configuration numbers
	 * all are in the range 0...getNumConfigs()-1.
	 */
	static unsigned long getNumConfigs();

	/**
	 * Return the number of boxes.
	 */
	static unsigned int numBoxes();

	/**
	 * Default constructor: initial configuration
	 */
	Config();

	/**
	 * Constructor: creates a configuration with the specified configuration number.
	 */
	Config(unsigned long confNo);

	/**
	 * Destructur: deallocate memory.
	 */
	~Config();

	/**
	 * Returns the configuration number for this configuration.
	 */
	unsigned long getConfig();

	/**
	 * Updates this configuration to the one with the specified number.
	 */
	void setConfig(unsigned long confNo);

	/**
	 * If a valid successor configuration can be reached from the current configuration by moving
	 * the box 'box' into direction 'dir', the number of the new configuration is returned, else 'NONE'.
	 * 'box' is the number of the box to be moved (0...numBoxes()-1)
	 * 'dir' is the direction of movement (0...3)
	 * *newBox returns the (new) number of the moved box.
	 */
	unsigned long getNextConfig(unsigned int box, unsigned int dir, unsigned int * newBox);

	/**
	 * Is there a box on field 'pos' of the playfield?
	 * For reasons of efficiency, this method is declared as 'inline,
	 * i.e., its call will be replaced by the given code.
	 */
	inline bool hasBox(unsigned int pos)
	{
		return Playfield::isValid(pos) && ((boxes & (1L << pos)) != 0);
	}

	/**
	 * Can the player reach the field 'pos' of the playing field?
	 */
	bool isReachable(unsigned int pos);

	/**
	 * Print the configuration 'graphically'.
	 */
	void print();
	
 private:
	// Number of configurations just for the boxes (without player)
	static unsigned long nBoxConfigs;
	
	// Configuration number of the solution (all boxes are on their target positions)
	static unsigned long solutionConfNo;


	// Configuration number of this configuration
	unsigned long configNo;

	// Array storing the positions of the boxes on the playing field. This array is always
	// sorted according to the positions!
	unsigned int * boxPos;

	// Bit set with the positions of the boxes. I.e., if bit 'i' is set, there is a box on
	// field 'i' of the playing field.
	unsigned long boxes;

	// For for each position of the playing field, this array contains the connected component
	// associated with that position. The player can only move within its current connected
	// component. For each possible position of the player, this array therefore indicates
	// which other fields the player can reach.
	unsigned short * comp;


	// Computes 'boxes' from 'boxPos'.
	void initBoxesBitSet();
	
	// Checks whether the field with number 'pos' has no box on it.
	// For efficiency reasons, this method is declared inline, i.e., a call to this method is
	// replaced by a copy of the method's body.
	inline bool hasNoBox(int pos)
	{
		return (pos >= 64) || ((boxes & (1L << pos)) == 0);
	}

	// Move the 'box'-th box to the field with number 'newPos' and return the new
	// number of the box (since the boxes are always sorted according to their
	// position on the playing field).
	unsigned int moveBox(unsigned int box, unsigned int newPos);

	// Compute the connected components. See attribute 'comp'.
	void setComponents(unsigned short comp[]);

	// Can position 'pos' of the playing field be emptied? The argument 'path' is a
	// bit set to avoid cycles during the search. It is initialized with 0.
	bool canBeEmptied(unsigned int pos, unsigned long path);
};

