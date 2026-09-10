This program has two source files,  If you try to compile only like this

    gcc -Wall -std=C99 introC_3.c -o prog

then you will get the following error

	1 warning generated.
	Undefined symbols for architecture x86_64:
	  "_northmost", referenced from:
		  _main in introC_3-14ad94.o
	  "_simpleFunc1", referenced from:
		  _main in introC_3-14ad94.o
		  _main in introC_3-14ad94.o
	  "_simpleFunc2", referenced from:
		  _main in introC_3-14ad94.o
		  _main in introC_3-14ad94.o
	ld: symbol(s) not found for architecture x86_64

and then you will check and recheck that Line 14 of introC_3.c indeed includes the
header file with the prototypes for northmost, simpleFunc1, simpleFunc2 and you will
think:  "but these functions have been defined!"

In fact, the prototypes don't define the functions, they only *declare* them.
Essentially, when we write

	int simpleFunc1(int x, int* y);

we are simply telling the compiler: "Trust me.  There is somewhere in this program a
proper definition, with code, for a function named simpleFunc1 that takes as arguments
an int value and a pointer to int, and returns an int value."  The compiler takes note
of that ("OK, I'll take your word for it and I will enter simpleFunc1 in my table of
symbols"), so that when the compiler arrives at Lines 139-141:

	int m = 10, n = 25;
	printf("\tPoint 4.1: m = %4d   n = %4d\n", m, n);
	int k = simpleFunc1(m, &n);

it sees that you are calling that function simpleFunc1 you told it about earlier in
your forward declaration, that you are sending to it the value of m (an int value), its
address (a pointer to int), and store the value returned by the function into an int
variable.  Everything checks, so no error here.

But when gcc tries to put all the pieces together to produce an executable, it discovers
that it never saw code (a definition) for that simpleFunc1 symbol (same for simpleFunc2 and
northmost).  And that's what it's telling you:  "But you *told me* that these were defined
somewhere!"  A couple of things to take note of here:

	- there is no mention of a specific line number where the error would take place,
		because this is not a problem with syntax or semantics.
	- the mention of "ld: symbol(s)" tells you that it's a loader error.
	- If you care about details, you also see that the compiler adds an underscore in front
		of the symbol's name in its private data.  This is the reason why, unlike some
		authors, I never have variables or functions that start with an underscore,
		because I find that this is a source of confusion.  On the other hand, I use postfix 
		underscores, in C++, for private class members.
		
To see the difference with a real compiler error, simply replace ld: simpleFunc1 by simpleFuncX
on line 141.  Now we get the following error

	introC_3.c:141:10: error: call to undeclared function 'simpleFuncX'; ISO C99 and later do not support implicit function declarations [-Wimplicit-function-declaration]
			int k = simpleFuncX(m, &n);

This one is a syntax error message from the compiler.  You get a line number, and the compiler
tells you:  "I never heard of this symbol that you use."  Note here that here what is missing
is a declaration.  In the original loader error, what was missing was a definition.


Now that we know what went wrong, what is the solution?  Simply to ask the compiler to
look at the definition 

    gcc -Wall -std=C99 introC_3.c myFunctions.c -o prog
                       \_______________________/
                       list all the source files
                       
                       
                       
                       
                       

