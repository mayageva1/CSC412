//arguments
//1. The path to a text file containing binning data in the format presented in Subsection 2.4;
//2. Two integers defining a range of time values;
//3. The path to the file where to output your time slice average results.

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

typedef struct{
    int date;
    float temp;
} Record;
typedef struct{
    float minLongitude;
    float maxLongitude; 
    float minLatitude;
    float maxLatitude;
    int row; // bin row
    int col; // bin column
    int numRecords;
    Record *Records;
} Bin;

int main(int argc, char* argv[]){
    
    char* inputFilePath = argv[1];
    int startDate = atoi(argv[2]);
    int endDate = atoi(argv[3]);
    char* outputFilePath = argv[4];

    // Open the input file
    FILE* inputFile = fopen(inputFilePath, "r");
    float minLong, maxLong, minLat, maxLat;
    int numLongDiv, numLatDiv;
    fscanf(inputFile, "%f %f %d %f %f %d", &minLong, &maxLong, &numLongDiv, &minLat, &maxLat, &numLatDiv);
    
    //calculate bin dimensions
    float longBinWidth = (maxLong-minLong)/numLongDiv;
    float latBinHeight = (maxLat-minLat)/numLatDiv;
    
    // Allocate memory for bins
    Bin **bins = malloc(numLongDiv * sizeof(Bin *));
    for (int row = 0; row < numLongDiv; row++) {
        bins[row] = malloc(numLatDiv * sizeof(Bin));
        for (int col = 0; col < numLatDiv; col++) {
            bins[row][col].minLongitude = minLong + row * longBinWidth;
            bins[row][col].maxLongitude = bins[row][col].minLongitude + longBinWidth;

            bins[row][col].minLatitude  = minLat + col * latBinHeight;
            bins[row][col].maxLatitude  = bins[row][col].minLatitude + latBinHeight;

            bins[row][col].row = row;   // longitude index
            bins[row][col].col = col;   // latitude index
            bins[row][col].numRecords = 0;
            bins[row][col].Records = NULL;
        }
    }
    int numRecords;
    char tempChar;
    while((tempChar=fgetc(inputFile)) != EOF){
        fscanf(inputFile, "%d", &numRecords);
        if(numRecords <= 0) continue; // skip empty bins
        else{
            for(int i=0; i<numRecords; i++){
                int stationId, date;
                float longitude, latitude;
                fscanf(inputFile, "%d %f %f %d %f", &stationId, &longitude, &latitude, &date);
                if(date > startDate || date < endDate){
                    Record currentRecord;
                    currentRecord.date = date;
                    fscanf(inputFile, "%f", &currentRecord.temp);
                    // Find the appropriate bin for this record
                    int col = (longitude - minLong) / longBinWidth;
                    int row = (latitude - minLat) / latBinHeight;
                    if (col >= 0 && col < numLongDiv && row >= 0 && row < numLatDiv) {
                        // Add the record to the bin
                        Bin* bin = &bins[col][row];
                        bin->numRecords++;
                        bin->Records = (Record*)realloc(bin->Records, bin->numRecords * sizeof(Record));
                        bin->Records[bin->numRecords - 1] = currentRecord;
                    }
                }
            }
        }
    }
    // Read the binning data
    // ...


    // Close the input file
    fclose(inputFile);

    // Open the output file
    FILE* outputFile = fopen(outputFilePath, "w");

    // Write the time slice average results
    // ...
    for(int row=0; row<numLongDiv; row++){
        for(int col=0; col<numLatDiv; col++){
            if(bins[row][col].numRecords>1){
                Bin* bin = &bins[row][col];
                float totalTemp = 0.0;
                int totalTime = 0;
                for(int i=0; i<bin->numRecords; i++){
                    totalTemp += bin->Records[i].temp;
                    totalTime++;
                }
                float avgTemp = totalTemp / totalTime;
                float avgTime = (float)(startDate + endDate) / 2;
                fprintf(outputFile, "%d %.2f\n", bin->numRecords, avgTemp);
            }
            else{
                fprintf(outputFile, "0\n");
            }
        }
    }

    // Close the output file
    fclose(outputFile);
}
//gcc -Wall -std=c11 Programs/Version-3/prog02_3.c -o prog02_3
//./prog02_3 output2.txt 122456 230411 output3.txt