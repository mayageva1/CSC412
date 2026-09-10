/*****************************************************************************
 Excerpt from "Linux Programmer's Guide - Chapter 6"
 (C)opyright 1994-1995, Scott Burkett
 ***************************************************************************** 
 MODULE: fifoclient.c
 *****************************************************************************/

#include <string>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

using namespace std;

const string  FIFO_PATH = "/tmp/LAB08_FIFO";

int main(int argc, const char *argv[])
{
	FILE *fp;

	if((fp = fopen(FIFO_PATH.c_str(), "w")) == NULL) {
			perror("fopen");
			exit(1);
	}

	fputs(argv[1], fp);

	fclose(fp);
	return(0);
}

