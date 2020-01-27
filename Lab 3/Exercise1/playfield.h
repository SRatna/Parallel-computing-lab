using namespace std;

class Config;

/**
 * This class represents a given level of Sokoban, i.e., the playing field (board) with the
 * initial positions of the boxes and the player as well as the positions of the targets.
 * Internally, the playing field is not represented as a two-dimensional array, but in the form
 * of numbered fields:
 *  - 0 ... nBox-1       : Target fields (storage locations)
 *  - nBox ... nPos-1    : Other fields where boxes may be placed without creating a
 *                         dead-end (i.e., in principle it must still be possible to move
 *                         the box away from this field)
 *  - nPos ... nFields-1 : Fields that can only be occupied by the player.
 * The topologie ist stored in the array 'neighbors', which contains the left, upper, right,
 * and lower neighbor field for each field (if any, i.e., only if the field is not a wall).
 */
class Playfield
{
 private:
	// Matrix with the position numbers for each field (x,y) of the playing field
	// (for the output of a configuration)
	static unsigned int ** posNo;
	// Width of the playing field
	static unsigned int nx;
	// Height of the playing field
	static unsigned int ny;

	// Initializes the playing field from a textual representation. This consists of an
	// array for each row of the playing field, with a total of 'ny' rows.
	static void init(string field[], unsigned int ny);

 public:
	/**
	 * Special value for the 'neighbor' array, if there is no neighboring field
	 * in some direction (i.e., the neighboring field is a wall)
	 */
	static const unsigned int NONE = -1;

	/**
	 * Initial position of the player.
	 */
	static unsigned int initialPlayerPos;
	
	/**
	 *  Initial positions of the boxes
	*/
	static unsigned int * initialBoxPos;
	
	/**
	 * Positions of the targets (storing locations)
	 */
	static unsigned int * goalPos;

	/**
	 * Positions of the neighboring fields of a field. The indices 0 .. 3 here mean the
	 * left, upper, right, and lower neighboring field.
	 */
	static unsigned int * neighbor[4];
	
	/**
	 * Number of boxes.
	 */
	static unsigned int nBox;
	
	/**
	 * Number of fields that may contain a box.
	 */
	static unsigned int nPos;
	
	/**
	 * Total number of fields.
	 */
	static unsigned int nFields;
   
	// ==================================================================

	/**
	 * Initializes the playing field from the given file.
	 */
	static void init(const char * fname);
		
	/**
	 * Is the given position valid, i.e., not a wall?
	 * For efficiency reasons, this method is declared inline, i.e., a call to this method is
	 * replaced by a copy of the method's body.
	 */
	static inline bool isValid(unsigned int pos)
	{
		return pos != NONE;
	}

	/**
	 * Is the given position a target?
	 * For efficiency reasons, this method is declared inline, i.e., a call to this method is
	 * replaced by a copy of the method's body.
	 */
	static inline bool isGoal(unsigned int pos)
	{
		return pos < nBox;
	}

	/**
	 * Is the given position a dead-end? I.e., the position is not a target and a
	 * box at this position cannot be moved away again.
	 * For efficiency reasons, this method is declared inline, i.e., a call to this method is
	 * replaced by a copy of the method's body.
	 */
	static inline bool isDead(unsigned int pos)
    {
		return pos >= nPos;
	}

	/**
	 * Print a configuration 'graphically'.
	 */
	static void print(Config * conf);
};

