//arguments
//1. The path to a text file containing binning data in the format presented in Subsection 2.4;
//2. The longitude and latitude (two floating point numbers) of a point you want to query about;
//3. The path to the file where to output the results of your query.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int date;
    float temp;
} Record;
typedef struct{
    float minLong;
    float maxLong; 
    float minLat;
    float maxLat;
    int row; // bin row
    int col; // bin column
    int numRecords;
    Record *Records;
} Bin;
int main(int argc, char* argv[]){
    //parse CLA
    char* inputFilePath = argv[1];
    float queryLong = atof(argv[2]);
    float queryLat = atof(argv[3]);
    char* outputFilePath = argv[4];
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    // Read header info
    float minLong, maxLong, minLat, maxLat;
    int numLongDiv, numLatDiv;
    fscanf(inputFile, "%f %f %d %f %f %d",
           &minLong, &maxLong, &numLongDiv, &minLat, &maxLat, &numLatDiv);
    fprintf(outputFile, "%.2f %.2f %d %.2f %.2f %d\n",
            minLong, maxLong, numLongDiv, minLat, maxLat, numLatDiv);
    
    //bin width and height
    float longBinWidth = (maxLong-minLong)/numLongDiv;
    float latBinHeight = (maxLat-minLat)/numLatDiv;

    //allocate memory for bins
    Bin **bins = malloc(numLongDiv * sizeof(Bin *));
        for (int row = 0; row < numLongDiv; row++) {
            bins[row] = malloc(numLatDiv * sizeof(Bin));
            for (int col = 0; col < numLatDiv; col++) {
                bins[row][col].minLong = minLong + row * longBinWidth;
                bins[row][col].maxLong = bins[row][col].minLong + longBinWidth;
                bins[row][col].minLat  = minLat + col * latBinHeight;
                bins[row][col].maxLat  = bins[row][col].minLat + latBinHeight;
                bins[row][col].row = row;   // longitude index
                bins[row][col].col = col;   // latitude index
                bins[row][col].numRecords = 0;
                bins[row][col].Records = NULL;
            }
        }
    //go thru each bin
    int numRecords;
    while (fscanf(inputFile, "%d", &numRecords) == 1) {
        //point is out of query bin range
        if (numRecords == 0) {
            fprintf(outputFile, "0\n");
        }
        //point is in query bin range
        else{
            for(int i=0; i<numRecords; i++){
                int id, date;
                float longitude, latitude, temp;
                fscanf(inputFile, "%d %f %f %d %f", &id, &longitude, &latitude, &date, &temp);
                fscanf(inputFile, "%*[^\n]");
                fgetc(inputFile);

                int row = (int)((longitude - minLong) / longBinWidth);
                int col = (int)((latitude - minLat) / latBinHeight);
                if (row < 0) row = 0;
                if (row >= numLongDiv) row = numLongDiv - 1;
                if (col < 0) col = 0;
                if (col >= numLatDiv) col = numLatDiv - 1;
                //check if the record is within the binning region
                if(longitude < minLong || longitude > maxLong ||
                    latitude < minLat || latitude > maxLat) {
                    continue; // skip this record
                }

                
                //add record to bin
                Bin *bin = &bins[row][col];
                bin->numRecords++;
                bin->Records = (Record*)realloc(bin->Records, bin->numRecords * sizeof(Record));
                bin->Records[bin->numRecords - 1].date = date;
                bin->Records[bin->numRecords - 1].temp = temp;
            }
        }

    }
 

    //gcc -Wall -std=c11 Programs/Version-4/prog02_4.c -o prog02_4
    //./prog02_4 Data/bin1.txt 34.52 -72.34 Output/binQuery1.txt