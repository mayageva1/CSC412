//
//  gl_frontEnd.h
//  GL threads
//
//  Created by Jean-Yves Hervé on 2025-10-27
//

#ifndef GL_FRONT_END_H
#define GL_FRONT_END_H

#include "glPlatform.h"


//-----------------------------------------------------------------------------
//	Custom data types
//-----------------------------------------------------------------------------

enum ColorLabel {
	BLACK_COL = 0,
	WHITE_COL,
	BLUE_COL,
	GREEN_COL,
	YELLOW_COL,
	RED_COL,
	//
	NUM_COLORS
};

//	Rules of the automaton (in C, it's a lot more complicated than in
//	C++/Java/Python/Swift to define an easy-to-initialize data type storing
//	arrays of numbers.  So, in this program I hard-code my rules
enum class Rules {
	GAME_OF_LIFE_RULE,
	CORAL_GROWTH_RULE,
	AMOEBA_RULE,
	MAZE_RULE
};
//#define GAME_OF_LIFE_RULE	1
//#define CORAL_GROWTH_RULE	2
//#define AMOEBA_RULE			3
//#define MAZE_RULE			4


//-----------------------------------------------------------------------------
//	Function prototypes
//-----------------------------------------------------------------------------

void initializeFrontEnd(int argc, const char** argv,
						void (*gridCB)(void),
						void (*stateCB)(void),
						void (*timerCB)(int));

//	Functions implemented in main.cpp but called by the glut callback functions
void resetGrid(void);
void oneGeneration(void);
void myTimerFunc(int val);
void drawGrid(unsigned short**grid, unsigned short numRows, unsigned short numCols);
void drawState(unsigned short numLiveThreads);
void myDisplayFunc(void);

#endif // GL_FRONT_END_H

