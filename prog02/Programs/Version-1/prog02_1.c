#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int idNum;
    float longitude;
    float latitude;
    int date;
    float temp;
    char *comment;
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

// Function to sort records in a bin
void sortRecords(Record* records, int numRecords){
    for(int i=0; i<numRecords-1; i++){
        for(int j=0; j<numRecords-i-1; j++){
            if(records[j].date > records[j+1].date ||
               (records[j].date == records[j+1].date && records[j].latitude > records[j+1].latitude) ||
               (records[j].date == records[j+1].date && records[j].latitude == records[j+1].latitude && records[j].longitude > records[j+1].longitude)){
                Record temp = records[j];
                records[j] = records[j+1];
                records[j+1] = temp;
            }
        }
    }
}

// Returns malloc'd string (empty if none)
char* getComment(const char* line){
    char commentBuf[256] = "";
    int int1; float float1, float2; int int2; float float3;
    int n = sscanf(line, "%d %f %f %d %f %[^\n]", &int1, &float1, &float2,
           &int2, &float3, commentBuf);
    //fprintf(stderr, "Comment extracted: '%s'\n", commentBuf);
    if(n < 6) commentBuf[0] = '\0'; // no comment found
    return strdup(commentBuf); // malloc'd string (empty if none)
}


//arguments


//The path to text file w/ list of paths to individual data record file
//The min and maxi longitude val of the binning region, 
//and the number of binning divisions along this direction;
//The min and max latitude values of the binning region
//and the number of binning divisions along this direction;
//The path to the file where to output your binning results.

int main(int argc, char* argv[]){
    //parse CLA
    char* inputFilePath = argv[1];
    float minLong = atof(argv[2]);
    float maxLong = atof(argv[3]);
    int numLongDiv = atoi(argv[4]);
    float minLat = atof(argv[5]);
    float maxLat = atof(argv[6]);
    int numLatDiv = atoi(argv[7]);
    char* outputFilePath = argv[8];

    float longBinWidth = (maxLong-minLong)/numLongDiv;
    float latBinHeight = (maxLat-minLat)/numLatDiv;
    //create 2D array of bins
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

    //open file list and store the paths
    FILE* inputFile = fopen(inputFilePath, "r");
    int numPaths;
    fscanf(inputFile,"%d",&numPaths);
    char **filePaths  = (char**)malloc(numPaths*sizeof(char*));
    char buffer[100];
    for(int i=0;i<numPaths;i++){
        fscanf(inputFile,"%s",buffer);
        filePaths[i] = (char*)malloc((strlen(buffer)+1)*sizeof(char));
        strcpy(filePaths[i],buffer);
    }
    fclose(inputFile);


    //loop through each file path

    for(int j=0; j<numPaths; j++){
        char* weatherFilePath = filePaths[j];
        FILE* weatherFile = fopen(weatherFilePath, "r");
        if(weatherFile == NULL){
            fprintf(stderr, "Error opening file: %s\n", weatherFilePath);
            continue; // skip to next file
        }
        //open the weather data, read one line
        char lineBuffer[256];
        while(fgets(lineBuffer, sizeof(lineBuffer), weatherFile)) {
            Record currentRecord;
            int n = sscanf(lineBuffer, "%d %f %f %d %f",
            &currentRecord.idNum,
            &currentRecord.longitude,
            &currentRecord.latitude,
            &currentRecord.date,
            &currentRecord.temp);
            if (n < 5) {
                //fprintf(stderr, "Skipping malformed line: %s", lineBuffer);
                continue; // skip malformed lines
            }
            currentRecord.comment = getComment(lineBuffer);
            // handle comment
        
            //check if the record is within the binning region
            if(currentRecord.longitude < minLong || currentRecord.longitude > maxLong ||
                currentRecord.latitude < minLat || currentRecord.latitude > maxLat) {
                free(currentRecord.comment);                
                continue; // skip this record
            }
            //compute which bin (row,col) the record belongs to
            //add the record to the bin
            int row = (int)((currentRecord.longitude - minLong) / longBinWidth);
            int col = (int)((currentRecord.latitude - minLat) / latBinHeight);
            if (row < 0) row = 0;
            if (row >= numLongDiv) row = numLongDiv - 1;
            if (col < 0) col = 0;
            if (col >= numLatDiv) col = numLatDiv - 1;

            bins[row][col].Records = realloc(bins[row][col].Records,(bins[row][col].numRecords + 1) * sizeof(Record));
            bins[row][col].Records[bins[row][col].numRecords] = currentRecord;
            bins[row][col].numRecords++;
            
        }
        fclose(weatherFile);
    }
        
        
    //sort each bin by date ascending, if tie on date: sort by latitude and then longitude ascending

    for (int row = 0; row < numLongDiv; row++) {
        for (int col = 0; col < numLatDiv; col++) {
            sortRecords(bins[row][col].Records, bins[row][col].numRecords);
        }
    }

    //Categories -> size = (Categories -> size * 100) // 100

    //write  everything to output file
    FILE* outputFile = fopen(outputFilePath, "w");
    fprintf(outputFile, "%.2f %.2f %d %.2f %.2f %d\n",
        minLong, maxLong, numLongDiv, minLat, maxLat, numLatDiv);

    for (int row = 0; row < numLongDiv; row++) {       // longitude rows
        for (int col = 0; col < numLatDiv; col++) {    // latitude cols
            //fprintf(stderr, "Bin[%d][%d]: %d records\n", row, col, bins[row][col].numRecords);
            fprintf(outputFile, "%d\n", bins[row][col].numRecords);
            for (int k = 0; k < bins[row][col].numRecords; k++) {
                Record rec = bins[row][col].Records[k];
                fprintf(outputFile, "%d %.2f %.2f %d %.2f",
                    rec.idNum, rec.longitude, rec.latitude, rec.date, rec.temp);
                if (rec.comment && strlen(rec.comment) > 0) {
                    fprintf(outputFile, " %s\n", rec.comment);
                } else {
                    fprintf(outputFile, "\n");
                }
            }
        }
    }
    fclose(outputFile);

}

//gcc -Wall -std=c11 Programs/Version-1/prog02_1.c -o prog02_1
//./prog02_1 Data/fileList.txt 40.0 150.0 5 60.0 130.0 5 output2.txt