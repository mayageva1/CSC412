#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h> 

int main(int argc, char* argv[])
{
	//	Here I hard-code some tasks, as list of strings.
	char* TASK_LIST[] = {
							"./Imagelibrary/executables/flipV Images/cells.tga Output",
							"./Imagelibrary/executables/flipH Images/peppers.tga Output",
							"./Imagelibrary/executables/channel -r Images/cells.tga Output",
							"./Imagelibrary/executables/channel -b Images/clown.tga Output",
							"./Imagelibrary/executables/flipV Images/peppers.tga Output",
							"./Imagelibrary/executables/channel -r Images/Tyler.tga Output",
							"./Imagelibrary/executables/channel -b Images/clown.tga Output",
							"./Imagelibrary/executables/channel -g Images/Seurat.tga Output",
							"./Imagelibrary/executables/channel -r Images/Tyler.tga Output",
							"./Imagelibrary/executables/flipH Images/peppers.tga Output",
							"./Imagelibrary/executables/channel -b Images/cells.tga Output",
							"./Imagelibrary/executables/flipH Images/Seurat.tga Output"};
	//	Note, this only works with automatically-allocated arrays, not arrays
	//	allocated by calloc/malloc/new
	int NUM_TASKS = (int)(sizeof(TASK_LIST)/sizeof(char*));	
	
	//	The parent process loops to create all child processes
	
	for (int i=0; i<NUM_TASKS; i++)
	{
		system(TASK_LIST[i]);
	}
			
	return 0;
}
