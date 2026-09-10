A note on versions of C
========================
Whereas I always strive to use the latest version of C++ for which there exist compilers
in a stable, final version for *nix platforms (at the time of this writing, it's C++20),
when it comes to C I am much more cautious/conservative.

Historically, the main versions of C in use (nobody ever uses the original 1978 K&R C
anymore) are:
	- ANSI C / ISO C (also known as C89 or C90).  This is classical C.
		Note:  I discovered while writing these notes that the const modifier was
		already part of C89.  This surprised me to no end because I had never encountered
		it before I started coding in C++.  The specificities of the C const are 
		discussed in the Intro C - 8 sample.
	- C99 mostly adds:
		* variable are not anymore restricted to be defined at the beginning of a block
		  of code, but can be intermingled with the code.  In particular, we can now
		  declare the loop counter   for (int k=0; k<10; k++) {...}
		  A consequence of this is that code written in C99 and latter versions tend to
		  attempt to minimize the dynamic scope of variables.  For example, we try to
		  declare variables just before the spot where they are used for the first time.
		  So, rather than
		  		int x, y;
		  		...
		  		x = someFunction(10);
		  		...
		  		y = 2*x + 1;
		  we prefer
		  		int x = someFunction(10);
		  		...
		  		...
		  		int y = 2*x + 1;
		* single-line comments beginning with //
		* more numerical data types, 
		* a bool type (if you include the proper header), 
		* inline functions,
		* variable-length arrays (a feature that has been controversial enough to be
		  disabled, then re-enabled in subsequent versions),
		* a confusing list of compiling options.
	- C11 mostly adds support for threads and confusion about what is required and
		supported.
	- C17 did more of the same.
	
Unless you end up working in a C shop (in which case your company will have strict rules
on what version/subversion to use), you are most likely to run into C99 code.  

The sure signs of whether the code you see was written originally in ANSI C then cleaned up 
for C99 are:
	- use of int instead of bool type (as in some of my sample code),
	- very few // comments, only /* ... */,
	- all variables, including loop counters, declared on top of the function.
This is the case of > 90% of all system code you are going to run into this semester.

On the other hand, not using VLA, as in
	void myFunc(int n) {
		float vals[n];		// this is a VLA automatic allocation
		....
	}
may not be a sign of anything more than a dislike of VLA (e.g. see Torvalds's comments on the topic).
	
This semester, we will compile all your programs for the C99 dialect.  If you use features 
from C11, then your code will not compile and you won't get any points.


How to compile and run the program from the CLI
===============================================

0) Please note that all my source files are meant to be viewed with a tab length = 4.
You can change this is the preferences/settings of your favorite text editor.  Or simply
"detab" my code to replace my tabs with the number of space characters that you prefer.


1) Create a folder where you will build and run the program.
You can of course build in the folder that the code came into when
you expanded the archive.  Generally, I would recommend that you
always build your programs in separate folders.  As your programs get
more complex, building the app will result in the creation of a bunch
of object files, and you don't want to get those mixed with your other
documents

2) cd into that folder

3) build the executable:
       gcc  -Wall -std=c99 introC-0.c -o myApp
    
   gcc			is the C compiler
   -Wall		says that I want all warnings on.  
   -std=c99		says that we want to use the C99 dialect
   introC-0.c  is the source file to compile
   -o is the gcc option that lets you select the name of the output
   myApp is the name of the executable your want to build [feel free to use another name]

4) execute the program

   ./myApp
   
   In the Unix file system (that we will study this semester),  "."  refers to the
   current folder.  By default,  "." is not on your $PATH configuration list that
   stores all the folders where to look for executables to run.  You could
   add "." to $PATH, but this is not recommended, for security reasons.  This
   is why when you execute from the console, you need to explicitly say where
   to find the executable you want to run.

5) To execute a program with some arguments (if the program is ready to handle them)

   ./myApp arg1 arg2 arg3
   
