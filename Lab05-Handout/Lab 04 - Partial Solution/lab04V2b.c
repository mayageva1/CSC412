#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h> 

void childProcess(int index, char* commandStr);

int main(int argc, char* argv[])
{
	//	Here I hard-code some tasks, as list of strings
	//	Since in this version all my "rotate" write the same image, I comment
	//	out some commands to only have one of each rotate/split/crop
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
		int p = fork();
		
		if (p==0)
		{
			//	Note 1: If I had made my TASK_LIST array a file-level global 
			//			variable, I would only need to pass i.
			//	Note 2:	The child is not supposed to come back from childProcess
			//			That function should end with an exit call.
			childProcess(i, TASK_LIST[i]);
		}
		else if (p<0)
		{
			printf("Something went wrong while creating Child #%d.  Bailing out.\n", i);
			exit(1);
		}
		childPid[i] = p;
	}
	
	sleep(5);
	
	//	Only the parent process arrives here.  It waits for al its child processes
	//	to terminate, not caring about the order.
	//	Note that if I cared to know the index of the child that finished, I could
	//	have stored the pid in an array and would now search for termProcess in it.
	pid_t termProcess;
	int statusVal;
	for (int i=0; i<NUM_TASKS; i++)
	{
		termProcess = waitpid(-1, &statusVal, 0);	
// 		termProcess = waitpid(childPid[NUM_TASKS-i-1/], &statusVal, 0);
		printf("Parent process got signal from child process %6d with status value %d --> %d\n", 
					termProcess, statusVal, WEXITSTATUS(statusVal));

	}
		
	return 0;
}

void childProcess(int index, char* commandStr)
{
	printf("Child Process #%d, with pid %d, will execute the command:\n\t%s\n",
			index, getpid(), commandStr);
	system(commandStr);
	
	exit(index);
}






		