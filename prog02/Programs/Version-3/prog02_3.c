#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
    char* inputFilePath = argv[1];
    int startDate = atoi(argv[2]);
    int endDate   = atoi(argv[3]);
    char* outputFilePath = argv[4];


    // open files
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    // Read header info
    float minLong, maxLong, minLat, maxLat;
    int numLongDiv, numLatDiv;
    fscanf(inputFile, "%f %f %d %f %f %d",
           &minLong, &maxLong, &numLongDiv, &minLat, &maxLat, &numLatDiv);
    fprintf(outputFile, "%.2f %.2f %d %.2f %.2f %d\n",
            minLong, maxLong, numLongDiv, minLat, maxLat, numLatDiv);

    //go thru each bin
    int numRecords;
    while (fscanf(inputFile, "%d", &numRecords) == 1) {
        if (numRecords == 0) {
            fprintf(outputFile, "0\n");
            continue;
        }
 
        double sumTemp = 0.0;
        long long sumDate = 0;

        //for each record in the bin
        //numvalidrecords: records in the bin within the specified date range
        int numValidRecords = 0;
        for (int i = 0; i < numRecords; i++) {
            int id, date;
            float longitude, latitude, temp;

            // Read record
            fscanf(inputFile, "%d %f %f %d %f",&id, &longitude, &latitude, &date, &temp);

            fscanf(inputFile, "%*[^\n]");
            fgetc(inputFile);
            if (date >= startDate && date <= endDate) {
                numValidRecords++;
                sumDate += date;
                sumTemp += temp;
            }
        }
        if (numValidRecords == 0) {
            fprintf(outputFile, "0\n");
        } else {
            double avgDate = round((double)sumDate / numValidRecords);
            double avgTemp = round((sumTemp / numValidRecords) * 100) / 100.0;
            fprintf(outputFile, "1\n%.0f %.2f\n", avgDate, avgTemp);
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}
//gcc -Wall -std=c11 Programs/Version-3/prog02_3.c -o prog02_3
//./prog02_3 output2.txt 122456 230411 output3.txt