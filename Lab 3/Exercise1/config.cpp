#include <iostream>
#include <stdlib.h>

#include "converter.h"
#include "config.h"

using namespace std;


unsigned long Config::nBoxConfigs;
unsigned long Config::solutionConfNo;
	

// ==================================================================

static unsigned int log2(unsigned long num)
{
	unsigned res = 0;
	for (; num > 0; num >>= 1)
		res++;
	return res;
}

// ==================================================================

/**
 * Initialization: The file 'fname' contains a string representation of the Sokoban level,
 * i.e., the initial configuration. The return value is the start configuration.
 */
Config * Config::init(const char * fname)
{
	Playfield::init(fname);
	Converter::init(Playfield::nPos, Playfield::nBox);
	nBoxConfigs = Converter::getNumConfigs();
	solutionConfNo = Converter::configToNo(Playfield::goalPos);
	
	cerr << "#Configs: " << getNumConfigs() << " (2^" << log2(getNumConfigs()) << ") "
		 << "#BoxConfigs: " << nBoxConfigs << " (2^" << log2(nBoxConfigs) << ") "
		 << "\n\n";
	
	return new Config();
}

/**
 * Does the specified configuration number represent a solution, i.e., are all boxes on
 * a target?
 */
bool Config::isSolutionConf(unsigned long conf)
{
	return (conf % nBoxConfigs) == solutionConfNo;
}
	
/**
 * Returns the maximum amount of configuration numbers. Thus, the configuration numbers
 * all are in the range 0...getNumConfigs()-1.
 */
unsigned long Config::getNumConfigs()
{
	return (1+3*Playfield::nBox) * nBoxConfigs;
}

/**
 * Return the number of boxes.
 */
unsigned int Config::numBoxes()
{
	return Playfield::nBox;
}

// ==================================================================

/**
 * Default constructor: initial configuration
 */
Config::Config()
{
	boxPos = new unsigned int[Playfield::nBox];
	comp = new unsigned short[Playfield::nFields];
	for (unsigned int i=0; i<Playfield::nBox; i++)
		boxPos[i] = Playfield::initialBoxPos[i];
	initBoxesBitSet();
	setComponents(comp);
	configNo = Converter::configToNo(boxPos) + comp[Playfield::initialPlayerPos] * nBoxConfigs;
}

/**
 * Constructor: creates a configuration with the specified configuration number.
 */
Config::Config(unsigned long confNo)
{
	boxPos = new unsigned int[Playfield::nBox];
	comp = new unsigned short[Playfield::nFields];
	setConfig(confNo);
}

/**
 * Destructur: deallocate memory.
 */
Config::~Config()
{
	delete[] comp;
	delete[] boxPos;
}

/**
 * Returns the configuration number for this configuration.
 */
unsigned long Config::getConfig()
{
	return configNo;
}
	
/**
 * Updates this configuration to the one with the specified number.
 */
void Config::setConfig(unsigned long confNo)
{
	Converter::noToConfig(confNo % nBoxConfigs, boxPos);
	initBoxesBitSet();
	setComponents(comp);
	configNo = confNo;
}

/**
 * If a valid successor configuration can be reached from the current configuration by moving
 * the box 'box' into direction 'dir', the number of the new configuration is returned, else 'NONE'.
 * 'box' is the number of the box to be moved (0...numBoxes()-1)
 * 'dir' is the direction of movement (0...3)
 * *newBox returns the (new) number of the moved box.
 */
unsigned long Config::getNextConfig(unsigned int box, unsigned int dir, unsigned int * newBox)
{
	unsigned int pos = boxPos[box];
	unsigned int playerPos = Playfield::neighbor[dir^2][pos];
	unsigned int newBoxPos = Playfield::neighbor[dir][pos];
	unsigned long result = NONE;
	
	if (isReachable(playerPos)
		&& Playfield::isValid(newBoxPos) && hasNoBox(newBoxPos)
		&& !Playfield::isDead(newBoxPos)) {
		box = moveBox(box, newBoxPos); // Execute the move
		// Check whether the box is on a target or can be removed again. If not, the move
		// leads to a dead-end and is not executed.
		if (Playfield::isGoal(newBoxPos) || canBeEmptied(newBoxPos, 0L)) {
			unsigned long confNo = Converter::configToNo(boxPos);
			unsigned short lcomp[Playfield::nFields];
			setComponents(lcomp);
			unsigned int playerComp = lcomp[pos];
			result = confNo + playerComp * nBoxConfigs;
			if (newBox != NULL)
				*newBox = box;
		}
		moveBox(box, pos); // Undo the move
	}
	return result;
}

/**
 * Can the player reach the field 'pos' of the playing field?
 */
bool Config::isReachable(unsigned int pos)
{
	int playerComp = configNo / nBoxConfigs;
	return Playfield::isValid(pos) && (comp[pos] == playerComp);
}

/**
 * Print the configuration 'graphically'.
 */
void Config::print()
{
	cout << "Config " << configNo << "(" << (configNo / nBoxConfigs) << ","
		 << (configNo % nBoxConfigs) << "/" << nBoxConfigs << ")\n";
	Playfield::print(this);
}

// ==================================================================

// Computes 'boxes' from 'boxPos'.
void Config::initBoxesBitSet()
{
	boxes = 0;
	for (unsigned int p=0; p<Playfield::nBox; p++) {
		if (boxPos[p] >= 64) {
			cerr << "Boxes: pos >= 64!\n";
			exit(1);
		}
		boxes |= (1L << boxPos[p]);
	}
}

// Move the 'box'-th box to the field with number 'newPos' and return the new
// number of the box (since the boxes are always sorted according to their
// position on the playing field).
unsigned int Config::moveBox(unsigned int box, unsigned int newPos)
{
	unsigned int oldPos = boxPos[box];
	unsigned int newBoxPos[Playfield::nBox];
	unsigned int j = 0;
	unsigned int newBox = -1;
	for (unsigned int i=0; i<Playfield::nBox; i++) {
		if (j == box)
			j++;
		if ((j < Playfield::nBox) && ((boxPos[j] < newPos) || (newBox != -1))) {
			newBoxPos[i] = boxPos[j++];
		}
		else {
			newBoxPos[i] = newPos;
			newBox = i;
		}
	}
	for (unsigned int i=0; i<Playfield::nBox; i++)
		boxPos[i] = newBoxPos[i];
	boxes &= ~(1L << oldPos);
	boxes |= (1L << newPos);
	return newBox;
}

// Compute the connected components. See attribute 'comp'.
void Config::setComponents(unsigned short comp[])
{
	unsigned int queue[Playfield::nFields];
	unsigned int in = 0;
	unsigned int out = 0;
	unsigned int cn = 0;
	const unsigned short none = -1;

	for (unsigned int i=0; i<Playfield::nFields; i++)
		comp[i] = none;

	for (unsigned int i=0; i<Playfield::nFields; i++) {
		if ((comp[i] == none) && hasNoBox(i)) {
			comp[i] = cn;
			queue[in++] = i;
			while (out < in) {
				unsigned int pos = queue[out++];
				for (unsigned int dir=0; dir<4; dir++) {
					unsigned int n = Playfield::neighbor[dir][pos];
					if (Playfield::isValid(n) && (comp[n] == none) && hasNoBox(n)) {
						comp[n] = cn;
						queue[in++] = n;
					}
				}
			}
			cn++;
		}
	}
}

// Can position 'pos' of the playing field be emptied? The argument 'path' is a
// bit set to avoid cycles during the search. It is initialized with 0.
bool Config::canBeEmptied(unsigned int pos, unsigned long path)
{
	if (!Playfield::isValid(pos))
		return false;
	if (hasNoBox(pos))
		return true;
	if ((path & (1L<<pos)) != 0)
		return false;
	path |= (1L<<pos);
	return (canBeEmptied(Playfield::neighbor[0][pos], path)
			&& canBeEmptied(Playfield::neighbor[2][pos], path))
		|| (canBeEmptied(Playfield::neighbor[1][pos], path)
			&& canBeEmptied(Playfield::neighbor[3][pos], path));
}

