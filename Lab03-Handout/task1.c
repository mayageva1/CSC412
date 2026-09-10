#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
	char* name;
	int size;
	char** words;
} Category;

int main(int argc, char* argv[]){
	
	// hard-coded path to the data file(s)
	// char filePath[] = "./Data/data1.txt";
	char* filePath = argv[1];
	// open the file and read the data
	FILE* fp  = fopen(filePath, "r");
	int fileSize;
	fscanf(fp,"%d", &fileSize);
	//fscan sees space, tab, newline all as whitespace. just jumps from one token to another
	//printf("size = %d\n\n",);

	Category *Categories = (Category*) calloc(fileSize, sizeof(Category));
	for (int i=0; i<fileSize; i++){
		char name[100];
		fscanf(fp,"%s%d",name, &Categories[i].size);
		Categories[i].name = (char*) calloc(strlen(name)+1, sizeof(char));
		
	}
	
	// Categories -> size = (Categories -> size * 100) // 100
	
	
	fclose(fp); 
	


	// some output



	return 0;
}