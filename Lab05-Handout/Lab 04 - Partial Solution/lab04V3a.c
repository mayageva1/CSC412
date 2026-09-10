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
	//	out some commands to only have one of each flipV, flipH, channel
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
	}
	
	//	Only the parent process arrives here.  It waits for al its child processes
	//	to terminate, not caring about the order.
	//	Note that if I cared to know the index of the child that finished, I could
	//	have stored the pid in an array and would now search for termProcess in it.
	pid_t termProcess;
	int statusVal;
	for (int i=0; i<NUM_TASKS; i++)
	{
		termProcess = waitpid(-1, &statusVal, 0);
		printf("Parent process got signal from child process %6d with status value %d --> %d\n", 
					termProcess, statusVal, WEXITSTATUS(statusVal));

	}
		
	return 0;
}

void childProcess(int index, char* commandStr)
{
	//	In this version, the child will perform an execXY(), except that 
	//	it is still a hard-coded exec command.  I am not fully parsing commandStr,
	//	just the first word (the actual command).
	
	//	first, extract the command. Read it into a pre-allocated, oversized array
	char command[16];
	sscanf(commandStr, "%s", command);
	
	//	And now, based on the command, run a different hard-coded version of execXY.
	//	Just for demo purposes, I use different forms of execXY for different cases.  
	//	In practice, if the number of arguments could only be determined after parsing
	//	commandStr, then I could only use execvp.
	if (strcmp(command, "./Imagelibrary/executables/flipV") == 0)
	{
		//	using execlp (note that paths are different because I run from within the
		//	Solution folder)
		//		           path                name      -------arguments-------  terminator
		execlp("./Imagelibrary/executables/flipV", 			//	path to executable
				"flipV prog", 								//	name
			   "Images/Renoir.tga", 				//	
			   "Output",							//	arguments
			   NULL);										//	terminator
		
		//	don't forget that nothing after the exec is reached.  There is no coming back. Ever.
		printf("flipV child exec failed\n");
		exit(1);
	}
	else if (strcmp(command, "./Imagelibrary/executables/flipH") == 0)
	{
		// using execlxp
		//		           path                name      -------arguments-------  terminator
		execlp("./Imagelibrary/executables/flipH", 				//	path to executable
				"flipH prog", 								//	name
			   "Images/Renoir.tga", 				//	
			   "Output",							//	arguments
			   NULL);										//	terminator
		
		//	don't forget that nothing after the exec is reached.  There is no coming back. Ever.
		printf("flipH child exec failed\n");
		exit(2);
	}
	else //	if (strcmp(command, "channel") == 0)
	{
		// using execvp
		char* commandVect[] = {
								"./Imagelibrary/executables/channel",	//	path to executable
								 "channel prog",					//	name
								 "-r",								//
								 "Images/Tyler.tga", 	//	arguments
								 "Output",				//	
								 NULL};								//	terminator
								 
		execvp(commandVect[0], commandVect+1);	
		//	don't forget that nothing after the exec is reached.  There is no coming back. Ever.
		printf("channel child exec failed\n");
		exit(3);
	}
	
	//	I keep the exit call for memories, but it is never reached.
	printf("This is not happening.  This is not happening.  This is not happening...\n");

}
		