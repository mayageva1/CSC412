#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h> 

int main(int argc, char* argv[])
{
	//	Here I hard-code some tasks, as a list of strings.
	char* TASK_LIST[] = {
							"./ImageLibrary/executables/flipV ./Images/Seurat.tga ./Output",
							"./ImageLibrary/executables/flipH ./Images/peppers.tga ./Output",
							"./ImageLibrary/executables/flipH ./Images/Renoir.tga ./Output",
							"./ImageLibrary/executables/gray ./Images/cells.tga ./Output",
//							"./ImageLibrary/executables/channel -r ./Images/cells.tga ./Output",
							"./ImageLibrary/executables/gray ./Images/clown.tga ./Output",
//							"./ImageLibrary/executables/channel -g ./Images/clown.tga ./Output",
//							"./ImageLibrary/executables/channel -g ./Images/Renoir.tga ./Output",
							"./ImageLibrary/executables/flipV ./Images/peppers.tga ./Output"};
														
	//	Note, this only works with automatically-allocated arrays, not arrays
	//	allocated by calloc/malloc/new
	int NUM_TASKS = (int)(sizeof(TASK_LIST)/sizeof(char*));	
	
	for (int k=0; k<NUM_TASKS; k++)
	{
		int pid = fork();
		if (pid < 0){
			
		}
		{
		// use "system" to execute the task
		

	}
	
		
	return 0;
}
		