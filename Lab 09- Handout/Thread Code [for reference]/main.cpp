//
//  main.cpp
//  This is a somewhat cleaned-up version of standard thread creation & joining,
//	to use for a C++ program.
//
//  Created by Jean-Yves Hervé on 2019-11-07.
//	Revised 2020-03-20

#include <thread>
#include <vector>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <cstring>       // for strerror()
#include <unistd.h>      // for usleep()
#include <sys/time.h>

using namespace std;

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Custom data types
//-----------------------------------------------------------------
#endif

// A struct data type to pass information to the thread and
//	possibly get values back from it.
struct  ThreadInfo {

    unsigned short	index;	
    //	application-dependent
    unsigned int timeToSleep;
};

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Function prototypes
//-----------------------------------------------------------------
#endif

void threadFunc (ThreadInfo info);


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Global variables
//-----------------------------------------------------------------
#endif

//	Vector of ThreadInfo structs.
vector<ThreadInfo> threadInfo;
//	Vector of threads
vector<thread> threadList;

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Function implementations
//-----------------------------------------------------------------
#endif


int main (int argc, char *argv[])
{
	unsigned short numThreads = 6;

	//	We are programming in C++, so I may as well allocate in C++ style
    threadInfo = vector<ThreadInfo>(numThreads);
	
	//	initialize all thread info structs that will be sent as argument to 
	//	the thread function
    for (unsigned short i = 0; i < numThreads; i++)
	{
		//	Note: the threadID field will be initialized by the pthread library
		//	when the thread is created.
        threadInfo[i].index = i;
	
	    //	application-dependent
    	threadInfo[i].timeToSleep = (numThreads - i)*500000;
	}
	
    // create the threads
    for (unsigned int i = 0; i < numThreads; i++)
	{
		cout << "main function now creating thread " << i << endl;

		//	Use smart pointers instead of
		//	thread myThread(threadInfo[i]);
 		threadList.push_back(thread(threadFunc, threadInfo[i]));
 		
 		//	Some threads are joinable, others aren't (are detached)
        if (i %2 == 10)
	    	threadList[i].detach();

    }

	//	At this point, all the threads have been created and
	//	are busy doing their thing
	//	Possibly, the main thread could have computations of its
	//	own to perform here. 
	usleep(1000000);

    //	Now rendezvous with all the non-detached threadInfo
    for (int i = 0; i<numThreads; i++) 
    {
    	//	Try to join all joinable threads (threads that didn't get detached)
        if (threadList[i].joinable())
        {
         	threadList[i].join();
        	cout << "Main thread joined thread " << i << endl;
        }
    }

	//	because I used smart pointers, all my objects get deleted when I leave the function
	return 0;
} // main


//	This is my thread function.
void threadFunc (ThreadInfo info)
{
	//------------------------------------------------
	//	Part 1: Extract argument data
	//------------------------------------------------
	cout << "\tThread " << info.index << " starts execution" << endl;
	
	//------------------------------------------------
	//	Part 2: Time to do some work
	//------------------------------------------------
	usleep(info.timeToSleep);
	
	//------------------------------------------------
	//	Part 3: Time to leave
	//------------------------------------------------
	cout << "\tThread " << info.index << " terminates execution" << endl;	
    return;
}

