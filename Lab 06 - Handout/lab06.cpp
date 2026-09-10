//
//  lab06.cpp
//  Lab 06 Code Handout

// "pure" C++ system headers
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>	//	C++ string class
// C++ wrappers for C system headers
#include <cstdio>
#include <cstring>	//	for C string functions strlen, etc.
//	C system headers with no C++ wrappers
#include <unistd.h>
#include <vector>
// user headers
#include "myHeader.h"



//	Some disapprove, but in a source file it's really a matter of 
//	personal style/preference.
using namespace std;

// #typedef double Number
using Number = double;

Number x = 10;

// typedef
// {
// 	int m;
// 	float x;
// } MyStruct;

struct MyStruct
{
	int m;
	float x;
};


//	Yes, now we can start using the const modifier!
int main(int argc, const char* argv[])
{
	vector<MyStruct> myVec;
	//--------------------------
	//	File Input reminder
	//--------------------------
	ifstream inFile;
	inFile.open("data.txt");
	cout << "Reading from the file... ";
	int n;
	inFile >> n;
	cout << n << " data points:" << endl;

	for (int k=0; k<n; k++)
	{
		//float x;
		//int m;
		MyStruct nameTemp;
		inFile >> nameTemp.x >> nameTemp.m;
		myVec.push_back(nameTemp);
		//cout << "\t" << m << "  " << x << endl;
	}
	inFile.close();
	for(int k=0; k<n; k++){
		cout << "\t" << myVec[k].m << "  " << myVec[k].x << endl;
	}

	return 0;
}
