#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

const string  FIFO_PATH = "/tmp/LAB08_FIFO";

//	You can go either way.  Pick the one you are more comfortable with.
#define VERSION_1	1		//	uses 	FILE* fp
#define VERSION_2	2		//	uses 	int fd
//
#define VERSION VERSION_1

struct Record {

	int zip;
	string label;
	int value;
};

//	My "database" is just a flat list of Record objects.
vector<Record> initDatabase(void);

int main(int argc, const char* argv[])
{
	//	I initialize my database.  If this were a real program I would do it
	//	with data read from a file.  Here, because I want to keep things simple
	//	in the lab, I just hard-coded my database.  
	vector<Record> database = initDatabase();

	//	Regardless of how the database was acquired, now we have to be able to handle 
	//	queries from client processes.
	
	//	This is the buffer I am going to read into
	char readbuf[BUFSIZ];

	//Create the FIFO if it does not exist */
	umask(0);
	mknod(FIFO_PATH.c_str(), S_IFIFO|0666, 0);
 
	bool keepGoing = true;
	while(keepGoing)
	{
		//	This version uses the fopen, fgets, etc. API.  These functions
		//	all support C strings, so when I do an fgets, it makes sure to write
		//	the string-terminating 0 at the end of the string I read.
		#if VERSION == VERSION_1

			FILE *fp;
			fp = fopen(FIFO_PATH.c_str(), "r");
			fgets(readbuf, BUFSIZ-1, fp);

		#else	//	v2

			//	This version uses the lower-level file IO library.  These functions
			//	(open, read, etc.) only know about bytes.  So if we want to interpret
			//	what we received as a string, we have to write the C string-ending 0 
			//	ourselves.  This can be done in two ways:
			//		- fill the entire buffer with 0 with memset prior to reading
			//			(that's what I do in the named pipe code sample that I had posted)
			//		- set to 0 the character after the last one read.  This is what
			//			I do here
			int fd = open(FIFO_PATH.c_str(), O_RDONLY);
			ssize_t size = read(fd, readbuf, BUFSIZ-1);
			readbuf[size] = '\0';
	
		#endif
				
		printf("Received string: %s\n", readbuf);	
			
		//	Do I really want to close now?  What would happen if a new client arrived between 
		//	now and the time when I reopen the pipe at the beginning of the loop?				
		#if VERSION == VERSION_1	
			fclose(fp);		
		#else		
			close(fd);		
		#endif

		//	But then this brings up the converse question:  What would happen if
		//	two or more clients write into the pipe between the time when I open it to read
		//	and the time I actually read.  Would I lose any information?

		int zip;
		sscanf(readbuf, "%d", &zip);
		printf("\tZIP code read: %d\n", zip);
			
		//	Now we want to search for the zip code in the database and print the info
		//	about the corresponding record.
		
	}

	
	//	As I am shutting down, I delete the pipe
	printf("Server shutting down the pipe...\n");
	if (unlink(FIFO_PATH.c_str()) != 0)
	{
		printf("Error unlinking the FIFO: ");
		switch (errno)
		{
			case EACCES:
				printf("You don't have permission\n");
				break;
			case EBUSY:
				printf("Resource busy\n");
				break;
			default:
				printf("One of ELOOP/ENAMETOOLONG/ENOENT/ENOTDIR/EPERM/EROFS\n");
				break;
		}
	}
	
	printf("Done.\n");

	return 0;
}x

// Quick and dirty initialization of my database
vector<Record> initDatabase(void)
{
	Record tempDB[] =  {{2882, "Narragansett, RI", 235},
						{2880, "Wakefield, RI", 437},
						{2881, "Kingston, RI", 367},
						{2817, "West Greenwich, RI", 435},
						{2818, "East Greenwich, RI", 682},
						{2864, "Cumberland, RI", 125}};
	size_t sizeDB = sizeof(tempDB)/sizeof(Record);

	vector<Record> database;
	for (size_t k=0; k<sizeDB; k++)
	{
		database.push_back(tempDB[k]);
	}
	
	return database;
}
