#include <iostream>
#include <stdlib.h>
#include <fstream>

#include "config.h"

// Ausgabe in Farbe. Für normale Ausgabe bitte auskommentieren.
#define COLOR

using namespace std;

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


// Symbols for the output of a configuration
static const char _wall = '#';
static const char _dead = '+';
static const char _goal = ':';
static const char _box = 'o';
static const char _goalBox = 'O';
static const char _empty = ' ';

static const char _player = 'm';
static const char _goalPlayer = 'M';
static const char _deadPlayer = '*';

static const char _uempty   = '-';

#ifdef COLOR
  // ANSI sequences to switch the color during output
  static const char * _red    = "[1;31m";
  static const char * _yellow = "[1;33m";
  static const char * _blue   = "[1;34m";
  static const char * _cyan   = "[1;36m";
  static const char * _normal = "[22;30m";
#else
  static const char * _red    = "";
  static const char * _yellow = "";
  static const char * _blue   = "";
  static const char * _cyan   = "";
  static const char * _normal = "";
#endif


// Matrix with the position numbers for each field (x,y) of the playing field
// (for the output of a configuration)
unsigned int ** Playfield::posNo;
// Width of the playing field
unsigned int Playfield::nx;
// Height of the playing field
unsigned int Playfield::ny;

/**
 * Initial position of the player.
 */
unsigned int Playfield::initialPlayerPos;

/**
 *  Initial positions of the boxes
 */
unsigned int * Playfield::initialBoxPos;

/**
 * Positions of the targets (storing locations)
 */
unsigned int * Playfield::goalPos;

/**
 * Positions of the neighboring fields of a field. The indices 0 .. 3 here mean the
 * left, upper, right, and lower neighboring field.
 */
unsigned int * Playfield::neighbor[4];

/**
 * Number of boxes.
 */
unsigned int Playfield::nBox;

/**
 * Number of fields that may contain a box.
 */
unsigned int Playfield::nPos;

/**
 * Total number of fields.
 */
unsigned int Playfield::nFields;

   
// ==================================================================

/**
 * Initializes the playing field from the given file.
 */
void Playfield::init(const char * fname)
{
	unsigned int n = 0;
	string line;

	// Open the file
	ifstream infile(fname);
	if (!infile) {
		cerr << "Error opening '" << fname << "'\n";
		exit(1);
	}

	// Determine the number of rows
	while (getline(infile, line))
		n++;
	infile.close();

	// Read the file
	string playfield[n+1];
	n = 0;
	infile.open(fname);
	while (getline(infile, playfield[n++]));
	infile.close();

	// Initialize the playing field
	init(playfield, n-1);
}

/**
 * the playing field from a textual representation. This consists of an
 * array for each row of the playing field, with a total of 'ny' rows.
 */
void Playfield::init(string field[], unsigned int any)
{
	unsigned int nGoal = 0;
	unsigned int playerX = -1;
	unsigned int playerY;

	// (1) Remember the position of the player and remove the player
	ny = any;
	nx = field[0].length();
	for (unsigned int y=0; y<ny; y++) {
		for (unsigned int x=0; x<nx; x++) {
			char c = field[y][x];
			if ((c == _player) || (c == _goalPlayer) || (c == _deadPlayer)) {
				if (playerX != -1) {
					cerr << "Error: more than one player!";
					exit(1);
				}
				playerX = x;
				playerY = y;
				if (c == _player) field[y][x] = _empty;
				else if (c == _goalPlayer) field[y][x] = _goal;
				else if (c == _deadPlayer) field[y][x] = _dead;
			}
		}
	}
	if (playerX < 0) {
		cerr << "Error: no player!\n";
		exit(1);
	}

	// (2) Determine the number of fields, boxes and targets.
	nFields = 0;
	nPos = 0;
	nBox = 0;
	for (unsigned int y=0; y<ny; y++) {
		for (unsigned int x=0; x<nx; x++) {
			char c = field[y][x];
			if (c != _wall) {
				if (c != _dead) {
					nPos++;
				}
				nFields++;
			}
			if ((c == _goal) || (c == _goalBox)) {
				nGoal++;
			}
			if ((c == _box) || (c == _goalBox)) {
				nBox++;
			}
		}
	}
	
	cerr << "#Boxes: " << nBox << ", #Pos: " << nPos << ", #Fields: " << nFields << "\n";
	
	if (nBox != nGoal) {
		cerr << "Error: #Boxes != #Goals!\n";
		exit(1);
	}

	// (3) Allocate the position arrays and initialize them
	unsigned int * xPos = new unsigned int[nFields];
	unsigned int * yPos = new unsigned int[nFields];
	posNo = new unsigned int*[ny];
	for (unsigned int y=0; y<ny; y++)
		posNo[y] = new unsigned int[nx];
	
	for (unsigned int y=0; y<ny; y++) {
		for (unsigned int x=0; x<nx; x++) {
			posNo[y][x] = NONE;
		}
	}

	// (4a) Enter the positions of the targets
	unsigned int i = 0;
	for (unsigned int y=0; y<ny; y++) {
		for (unsigned int x=0; x<nx; x++) {
			char c = field[y][x];
			if ((c == _goal) || (c == _goalBox)) {
				xPos[i] = x;
				yPos[i] = y;
				posNo[y][x] = i++;
			}
		}
	}

	// (4b) Enter the positions of the boxes
	for (unsigned int y=0; y<ny; y++) {
		for (unsigned int x=0; x<nx; x++) {
			char c = field[y][x];
			if ((c == _empty) || (c == _box)) {
				xPos[i] = x;
				yPos[i] = y;
				posNo[y][x] = i++;
			}
		}
	}

	// (4c) Enter the positions of the fields that may contain boxes
	for (unsigned int y=0; y<ny; y++) {
		for (unsigned int x=0; x<nx; x++) {
			char c = field[y][x];
			if (c == _dead) {
				xPos[i] = x;
				yPos[i] = y;
				posNo[y][x] = i++;
			}
		}
	}

	// (5) Compile the neighbor array
	for (i=0; i<4; i++)
		neighbor[i] = new unsigned int[nFields];
	for (i=0; i<nFields; i++) {
		neighbor[0][i] = posNo[yPos[i]][xPos[i]-1];
		neighbor[1][i] = posNo[yPos[i]-1][xPos[i]];
		neighbor[2][i] = posNo[yPos[i]][xPos[i]+1];
		neighbor[3][i] = posNo[yPos[i]+1][xPos[i]];
	}

	// (6a) Store the initial position of the player
	initialPlayerPos = posNo[playerY][playerX];
	
	// (6b) Store the initial position of the boxes
	initialBoxPos = new unsigned int[nBox];
	i = 0;
	for (unsigned int p=0; p<nPos; p++) {
		char c = field[yPos[p]][xPos[p]];
		if ((c == _box) || (c == _goalBox)) {
			initialBoxPos[i++] = p;
		}
	}
	
	// (6c) Store the positions of the targets
	goalPos = new unsigned int[nBox];
	i = 0;
	for (unsigned int p=0; p<nFields; p++) {
		char c = field[yPos[p]][xPos[p]];
		if ((c == _goal) || (c == _goalBox)) {
			goalPos[i++] = p;
		}
	}
}


/**
 * Print a configuration 'graphically'.
 */
void Playfield::print(Config * conf)
{
	for (unsigned int y=0; y<ny; y++) {
		for (unsigned int x=0; x<nx; x++) {
			unsigned int pos = posNo[y][x];
			bool box = conf->hasBox(pos);
			bool reachable = conf->isReachable(pos);
			if (!isValid(pos)) {
				// Wand
				cout << _normal << _wall;
			}
			else if (isGoal(pos)) {
				// Ziel mit oder ohne Kiste
				if (box)
					cout << _red << _goalBox;
				else
					cout << (reachable ? _blue : _cyan) << _goal;
			}
			else if (isDead(pos)) {
				// Sackgassen-Feld
				cout << (reachable ? _yellow : _cyan) << _dead;
			}
			else {
				if (box)
					cout << _red << _box;
				else if (reachable)
					cout << _normal << _empty;
				else
					cout << _cyan << _uempty;
			}
		}
		cout << "\n";
	}
	cout << _normal << "\n";
}
