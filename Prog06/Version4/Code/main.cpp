//
//  main.cpp
//  Cellular Automaton

 /*-------------------------------------------------------------------------+
 |	A graphic front end for a grid+state simulation.						|
 |																			|
 |	This application simply creates a glut window with a pane to display	|
 |	a colored grid and the other to display some state information.			|
 |	Sets up callback functions to handle menu, mouse and keyboard events.	|
 |	Normally, you shouldn't have to touch anything in the GUI code, unless	|
 |	you want to change some of the things displayed, add menus, etc.		|
 |	Only mess with this after everything else works and making a backup		|
 |	copy of your project.  OpenGL & glut are tricky and it's really easy	|
 |	to break everything with a single line of code.							|
 |																			|
 |	Current keyboard controls:												|
 |																			|
 |		- 'ESC' --> exit the application									|
 |		- space bar --> resets the grid										|
 |																			|
 |		- 'c' --> toggle color mode on/off									|
 |		- 'b' --> toggles color mode off/on									|
 |		- 'l' --> toggles on/off grid line rendering						|
 |																			|
 |		- '+' --> increase simulation speed									|
 |		- '-' --> reduce simulation speed									|
 |																			|
 |		- '1' --> apply Rule 1 (Conway's classical Game of Life: B3/S23)	|
 |		- '2' --> apply Rule 2 (Coral: B3/S45678)							|
 |		- '3' --> apply Rule 3 (Amoeba: B357/S1358)							|
 |		- '4' --> apply Rule 4 (Maze: B3/S12345)							|
 |																			|
 +-------------------------------------------------------------------------*/

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>  
#include <random>

//
#include "gl_frontEnd.h"

using namespace std;

#if 0
//---------------------------------------------
#pragma mark -
#pragma mark Custom data types
//---------------------------------------------
#endif

struct ThreadInfo{
	//
	//	whatever other input or output data may be needed
	//
	int threadIndex;   // index of the thread (0 .. numThreads-1)
	int startRow;      // first row (inclusive)
	int endRow;        // one past last row (exclusive)

	bool startWork;    // set by main: this thread should process its rows
	bool finished;     // set by thread: done with current generation
};

#if 0
//---------------------------------------------
#pragma mark -
#pragma mark Function prototypes
//---------------------------------------------
#endif

void displayGridPane(void);
void displayStatePane(void);
void initializeApplication(void);
void swapGrids(void);
unsigned short cellNewState(unsigned short i, unsigned short j);
// for Version 1 (short-lived threads)
void* threadFunc(void* arg);
void computeRange(int startRow, int endRow);

#if 0
//---------------------------------------------
#pragma mark -
#pragma mark Application-level global variables
//---------------------------------------------
#endif

//	Don't touch:  These are defined in gl_frontEnd.cpp
extern int GRID_PANE, STATE_PANE;
extern int gMainWindow, gSubwindow[2];

//	Precompiler #define to let us specify how things should be handled at the
//	border of the frame

#define FRAME_DEAD		0	//	cell borders are kept dead
#define FRAME_RANDOM	1	//	new random values are generated at each generation
#define FRAME_CLIPPED	2	//	same rule as elsewhere, with clipping to stay within bounds
#define FRAME_WRAP		3	//	same rule as elsewhere, with wrapping around at edges

//	Pick one value for FRAME_BEHAVIOR
#define FRAME_BEHAVIOR	FRAME_DEAD

//	The state grid and its dimensions.  We now have two copies of the grid:
//		- currentGrid is the one displayed in the graphic front end
//		- nextGrid is the grid that stores the next generation of cell
//			states, as computed by our threads.
unsigned short** currentGrid = nullptr;
unsigned short** nextGrid   = nullptr;

// Grid dimensions (set from command line)
unsigned short numRows = 0;
unsigned short numCols = 0;

// thread information
int numThreads = 0;
int maxNumThreads = 0;  

// arrays to hold pthread info
ThreadInfo* threadInfoArray = nullptr;
pthread_t* threadHandles    = nullptr;

//	the number of live threads (for the state pane display)
unsigned short numLiveThreads = 0;


// sleep time between generations (ms), changed with '+' and '-'
int generationSleepMilliSec = 50;

// Version 3 / Version 4
bool quitFlag = false;                      
pthread_mutex_t workMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  workCond  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  doneCond  = PTHREAD_COND_INITIALIZER;
int threadsAtBarrier = 0; // how many threads have finished this generation
bool workAvailable   = false; // main tells workers when a generation is ready to compute

// EC1: C++ random generator for initial cell states
std::mt19937 rng;
std::bernoulli_distribution cellAliveDist(0.5);   // 50% chance of "alive"

//	the rule currently selected by the user
Rules currentRule = Rules::GAME_OF_LIFE_RULE;

bool useColorMode = false;

int timerMilliSec = 100;

#if 0
//---------------------------------------------
#pragma mark -
#pragma mark Rendering functions
//---------------------------------------------
#endif

//==================================================================================
//	These are the functions that tie the simulation with the rendering.
//	Some parts are "don't touch."  Other parts need your intervention
//	to make sure that access to critical section is properly synchronized
//==================================================================================

void displayGridPane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[GRID_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render the grid.
	//---------------------------------------------------------
	drawGrid(currentGrid, numRows, numCols);
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	glutSetWindow(gMainWindow);
}

void displayStatePane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[STATE_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//---------------------------------------------------------
	drawState(numLiveThreads);
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	glutSetWindow(gMainWindow);
}

void myTimerFunc(int value)
{
	//	Re-prime the timer
	glutTimerFunc(timerMilliSec, myTimerFunc, value);

	//  possibly do something to update the state information displayed
    //	in the "state" pane
	
	//======================================================
	//	This call must **DEFINITELY** go away in Version 2.
	//======================================================
	//	 After you have properly multithreaded the code with "permanent" threads,
	//	the processing threads will run without having to be called within the
	//	rendering loop (of the main thread)
    // oneGeneration();
	
	glutSetWindow(gMainWindow);
	
	//	What I do here is ugly (the commented-out code is what should
	//	be done, but this is needed to circumvent a macOS problem.
	//glutPostRedisplay();
    myDisplayFunc();
    
}

#if 0
//---------------------------------------------
#pragma mark -
#pragma mark Main, init, and exit
//---------------------------------------------
#endif

//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function
//------------------------------------------------------------------------
int main(int argc, const char** argv)
{
	// We expect: programName <width> <height> <numThreads>
    if (argc != 4)
    {
        printf("Usage: %s <width(numCols)> <height(numRows)> <numThreads>\n", argv[0]);
        return 1;
    }

    // simple argument parsing
    int w = atoi(argv[1]);
    int h = atoi(argv[2]);
    int t = atoi(argv[3]);

    // basic validation
    if (w <= 5 || h <= 5)
    {
        printf("Error: width and height must be > 5\n");
        return 1;
    }
    if (t <= 0)
    {
        printf("Error: numThreads must be > 0\n");
        return 1;
    }
    if (t > h)
    {
        printf("Error: numThreads must not be larger than height (numRows)\n");
        return 1;
    }

    numCols       = static_cast<unsigned short>(w);
    numRows       = static_cast<unsigned short>(h);
    numThreads    = t;
    maxNumThreads = numThreads;
    numLiveThreads = static_cast<unsigned short>(numThreads);

	//initializing glut and the GUI.
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane, myTimerFunc);
	
	//application-level initialization
	initializeApplication();

    //	Version 4: create persistent worker threads that will keep randomly updating cells in-place
    for (int i = 0; i < numThreads; ++i)
    {
        int rc = pthread_create(&threadHandles[i],
                                nullptr,
                                threadFunc,
                                &threadInfoArray[i]);
        if (rc != 0)
        {
            printf("Error: pthread_create failed for thread %d\n", i);
            return 2;
        }
    }

	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	
	//	This will never be executed (the exit point will be in one of the
	//	call back functions).
	return 0;
}


void cleanupAndQuit(void)
{
    // Tell worker threads to exit their loop
    quitFlag = true;

    // Join all worker threads
    for (int t = 0; t < numThreads; ++t)
    {
        if (threadHandles != nullptr)
        {
            pthread_join(threadHandles[t], nullptr);
        }
    }

	//	Free allocated resource before leaving (not absolutely needed, but
	//	just nicer.  Also, if you crash there, you know something is wrong
	//	in your code.
    for (unsigned int i = 0; i < numRows; i++)
    {
        delete [] currentGrid[i];
        delete [] nextGrid[i];
    }
    delete [] currentGrid;
    delete [] nextGrid;

    // free thread arrays
    delete [] threadInfoArray;
    delete [] threadHandles;

    exit(0);
}




void initializeApplication(void)
{
    //  Allocate 2D grids based on numRows / numCols
    currentGrid = new unsigned short*[numRows];
    nextGrid    = new unsigned short*[numRows];
    for (unsigned short i = 0; i < numRows; i++)
    {
        currentGrid[i] = new unsigned short[numCols];
        nextGrid[i]    = new unsigned short[numCols];
    }

    // allocate thread info and pthread handles (Version 1)
    threadInfoArray = new ThreadInfo[numThreads];
    threadHandles   = new pthread_t[numThreads];

    // split rows as evenly as possible among threads
    int baseRows = numRows / numThreads;
    int extra    = numRows % numThreads;
    int currentStart = 0;

        for (int t = 0; t < numThreads; ++t)
    {
        int rowsForThisThread = baseRows + (t < extra ? 1 : 0);

        threadInfoArray[t].threadIndex = t;
        threadInfoArray[t].startRow    = currentStart;
        threadInfoArray[t].endRow      = currentStart + rowsForThisThread;

        // Version 2: initialize flags
        threadInfoArray[t].startWork = false;
        threadInfoArray[t].finished  = true;   // nothing to do yet

        currentStart += rowsForThisThread;
    }


    numLiveThreads = static_cast<unsigned short>(numThreads);

    // seed the C++ random generator (EC1)
    std::random_device rd;
    rng.seed(rd());

    // you can keep or remove srand; it's no longer used for resetGrid
    // srand((unsigned short) time(NULL));
	
    // fill grid with random initial state
    resetGrid();
}



#if 0
//---------------------------------------------
#pragma mark -
#pragma mark Implement this thread function
//---------------------------------------------
#endif

//---------------------------------------------------------------------
//---------------------------------------------------------------------

void* threadFunc(void* arg)
{
    ThreadInfo* info = static_cast<ThreadInfo*>(arg);

    // Simple per-thread random seed
    unsigned int seed = static_cast<unsigned int>(time(NULL)) ^ (info->threadIndex * 7331);

    while (!quitFlag)
    {
        // Pick a random cell
        unsigned short i = static_cast<unsigned short>(rand_r(&seed) % numRows);
        unsigned short j = static_cast<unsigned short>(rand_r(&seed) % numCols);

        // Compute new state based on current neighborhood
        unsigned short newState = cellNewState(i, j);

        // In-place update of currentGrid
        if (!useColorMode || newState == 0)
        {
            currentGrid[i][j] = newState;
        }
        else
        {
            // color reflects "age" of live cell
            if (currentGrid[i][j] < NUM_COLORS - 1)
                currentGrid[i][j] = currentGrid[i][j] + 1;
            else
                currentGrid[i][j] = currentGrid[i][j];  // stays very old
        }

        // Control speed of updates
        if (generationSleepMilliSec > 0)
        {
            usleep(generationSleepMilliSec * 1000); // ms -> microseconds
        }
    }

    return nullptr;
}



#if 0
//---------------------------------------------
#pragma mark -
#pragma mark CA functions
//---------------------------------------------
#endif

void resetGrid(void)
{
	for (unsigned short i = 0; i < numRows; i++)
	{
		for (unsigned short j = 0; j < numCols; j++)
		{
            // EC1: use C++ bernoulli_distribution instead of rand() % 2
			nextGrid[i][j] = cellAliveDist(rng) ? 1 : 0;
		}
	}
	swapGrids();
}


//	This function swaps the current and next grids, as well as their
//	companion 2D grid.  Note that we only swap the "top" layer of
//	the 2D grids.
void swapGrids(void)
{
	unsigned short** tempGrid = currentGrid;
	currentGrid = nextGrid;
	nextGrid = tempGrid;
}

//	I have decided to go for maximum modularity and readability, at the
//	cost of some performance.  This may seem contradictory with the
//	very purpose of multi-threading our application.  I won't deny it.
//	My justification here is that this is very much an educational exercise,
//	my objective being for you to understand and master the mechanisms of
//	multithreading and synchronization with mutex.  After you get there,
//	you can micro-optimi1ze your synchronized multithreaded apps to your
//	heart's content.
void computeRange(int startRow, int endRow)
{
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            unsigned short newState = cellNewState(
                static_cast<unsigned short>(i),
                static_cast<unsigned short>(j)
            );

            //	In black and white mode, only alive/dead matters
            //	Dead is dead in any mode
            if (!useColorMode || newState == 0)
            {
                nextGrid[i][j] = newState;
            }
            //	in color mode, color reflects the "age" of a live cell
            else
            {
                //	Any cell that has not yet reached the "very old cell"
                //	stage simply gets one generation older
                if (currentGrid[i][j] < NUM_COLORS - 1)
                    nextGrid[i][j] = currentGrid[i][j] + 1;
                //	An old cell remains old until it dies
                else
                    nextGrid[i][j] = currentGrid[i][j];
            }
        }
    }
}

void oneGeneration(void)
{
	//version 4: no need to do anything here


    // // Start a new generation: reset the barrier and tell worker threads to run
    // pthread_mutex_lock(&workMutex);

    // threadsAtBarrier = 0;
    // workAvailable    = true;

    // // Wake all worker threads so they can process their row ranges
    // pthread_cond_broadcast(&workCond);

    // // Wait until the last worker thread signals that the generation is done
    // while (workAvailable)
    // {
    //     pthread_cond_wait(&doneCond, &workMutex);
    // }

    // pthread_mutex_unlock(&workMutex);

    // // simple speed control: sleep between generations
    // if (generationSleepMilliSec > 0)
    // {	
    //     usleep(generationSleepMilliSec * 1000); // ms -> microseconds
    // }
}




//	Here I give three different implementations
//	of a slightly different algorithm, allowing for changes at the border
//	All three variants are used for simulations in research applications.
//	I also refer explicitly to the S/B elements of the "rule" in place.
unsigned short cellNewState(unsigned short i, unsigned short j)
{
	//	First count the number of neighbors that are alive
	//----------------------------------------------------
	//	Again, this implementation makes no pretense at being the most efficient.
	//	I am just trying to keep things modular and somewhat readable
	int count = 0;

	//	Away from the border, we simply count how many among the cell's
	//	eight neighbors are alive (cell state > 0)
	if (i>0 && i<numRows-1 && j>0 && j<numCols-1)
	{
		//	remember that in C, (x == val) is either 1 or 0
		count = (currentGrid[i-1][j-1] != 0) +
				(currentGrid[i-1][j] != 0) +
				(currentGrid[i-1][j+1] != 0)  +
				(currentGrid[i][j-1] != 0)  +
				(currentGrid[i][j+1] != 0)  +
				(currentGrid[i+1][j-1] != 0)  +
				(currentGrid[i+1][j] != 0)  +
				(currentGrid[i+1][j+1] != 0);
	}
	//	on the border of the frame...
	else
	{
		#if FRAME_BEHAVIOR == FRAME_DEAD
		
			//	Hack to force death of a cell
			count = -1;
		
		#elif FRAME_BEHAVIOR == FRAME_RANDOM
		
			count = rand() % 9;
		
				#elif FRAME_BEHAVIOR == FRAME_CLIPPED
	
			if (i>0)
			{
				if (j>0 && currentGrid[i-1][j-1] != 0)
					count++;
				if (currentGrid[i-1][j] != 0)
					count++;
				if (j<numCols-1 && currentGrid[i-1][j+1] != 0)
					count++;
			}

			if (j>0 && currentGrid[i][j-1] != 0)
				count++;
			if (j<numCols-1 && currentGrid[i][j+1] != 0)
				count++;

			if (i<numRows-1)
			{
				if (j>0 && currentGrid[i+1][j-1] != 0)
					count++;
				if (currentGrid[i+1][j] != 0)
					count++;
				if (j<numCols-1 && currentGrid[i+1][j+1] != 0)
					count++;
			}

			
	
		#elif FRAME_BEHAVIOR == FRAME_WRAPPED
	
			unsigned short 	iM1 = (i+numRows-1)%numRows,
							iP1 = (i+1)%numRows,
							jM1 = (j+numCols-1)%numCols,
							jP1 = (j+1)%numCols;
			count = currentGrid[iM1][jM1] != 0 +
					currentGrid[iM1][j] != 0 +
					currentGrid[iM1][jP1] != 0  +
					currentGrid[i][jM1] != 0  +
					currentGrid[i][jP1] != 0  +
					currentGrid[iP1][jM1] != 0  +
					currentGrid[iP1][j] != 0  +
					currentGrid[iP1][jP1] != 0 ;

		#else
			#error undefined frame behavior
		#endif
		
	}	//	end of else case (on border)
	
	//	Next apply the cellular automaton rule
	//----------------------------------------------------
	//	by default, the grid square is going to be empty/dead
	unsigned short newState = 0;
	
	//	unless....
	
	switch (currentRule)
	{
		//	Rule 1 (Conway's classical Game of Life: B3/S23)
		case Rules::GAME_OF_LIFE_RULE:

			//	if the cell is currently occupied by a live cell, look at "Stay alive rule"
			if (currentGrid[i][j] != 0)
			{
				if (count == 3 || count == 2)
					newState = 1;
			}
			//	if the grid square is currently empty, look at the "Birth of a new cell" rule
			else
			{
				if (count == 3)
					newState = 1;
			}
			break;
	
		//	Rule 2 (Coral Growth: B3/S45678)
		case Rules::CORAL_GROWTH_RULE:

			//	if the cell is currently occupied by a live cell, look at "Stay alive rule"
			if (currentGrid[i][j] != 0)
			{
				if (count > 3)
					newState = 1;
			}
			//	if the grid square is currently empty, look at the "Birth of a new cell" rule
			else
			{
				if (count == 3)
					newState = 1;
			}
			break;
			
		//	Rule 3 (Amoeba: B357/S1358)
		case Rules::AMOEBA_RULE:

			//	if the cell is currently occupied by a live cell, look at "Stay alive rule"
			if (currentGrid[i][j] != 0)
			{
				if (count == 1 || count == 3 || count == 5 || count == 8)
					newState = 1;
			}
			//	if the grid square is currently empty, look at the "Birth of a new cell" rule
			else
			{
				if (count == 1 || count == 3 || count == 5 || count == 8)
					newState = 1;
			}
			break;
		
		//	Rule 4 (Maze: B3/S12345)							|
		case Rules::MAZE_RULE:

			//	if the cell is currently occupied by a live cell, look at "Stay alive rule"
			if (currentGrid[i][j] != 0)
			{
				if (count >= 1 && count <= 5)
					newState = 1;
			}
			//	if the grid square is currently empty, look at the "Birth of a new cell" rule
			else
			{
				if (count == 3)
					newState = 1;
			}
			break;

		//  Rule 5 (HighLife: B36/S23)
        case Rules::HIGH_LIFE_RULE:

            // if the cell is currently alive, stay alive with 2 or 3 neighbors (same as GoL)
            if (currentGrid[i][j] != 0)
            {
                if (count == 2 || count == 3)
                    newState = 1;
            }
            // if the cell is dead, it is born with 3 or 6 neighbors
            else
            {
                if (count == 3 || count == 6)
                    newState = 1;
            }
            break;
		
		default:
			printf("Invalid rule number\n");
			exit(5);
	}

	return newState;
}

