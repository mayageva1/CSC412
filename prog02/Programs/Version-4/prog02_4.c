//arguments
//1. The path to a text file containing binning data in the format presented in Subsection 2.4;
//2. The longitude and latitude (two floating point numbers) of a point you want to query about;
//3. The path to the file where to output the results of your query.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    
    //bins of query point
    float longBinWidth = (maxLong-minLong)/numLongDiv;
    float latBinHeight = (maxLat-minLat)/numLatDiv;
    int queryLongBin = (int)((queryLong - minLong) / longBinWidth);
    int queryLatBin = (int)((queryLat - minLat) / latBinHeight);

    int numRecords;
    //go thru each bin
    while (fscanf(inputFile, "%d", &numRecords) == 1){
        
        if(numRecords==0){
            fprintf(outputFile, "0\n");
        }
        else{     
            double sumTemp = 0.0;
            long long sumDate = 0;
            int numValidRecords = 0;
            for(int i=0; i<numRecords; i++){
                int id, date;
                float longitude, latitude, temp;
                // Read record
                fscanf(inputFile, "%d %f %f %d %f",&id, &longitude, &latitude, &date, &temp);
                fscanf(inputFile, "%*[^\n]");
                fgetc(inputFile);

                if (longitude >= minLong + queryLongBin * longBinWidth &&
                longitude <  minLong + (queryLongBin + 1) * longBinWidth &&
                latitude  >= minLat  + queryLatBin  * latBinHeight &&
                latitude  <  minLat  + (queryLatBin  + 1) * latBinHeight){
                    numValidRecords++;
                    sumDate += date;
                    sumTemp += temp;
                }
            }
            //output 
            if (numValidRecords == 0){
                fprintf(outputFile, "0\n");
            }
            else{
                double avgDate = round((double)sumDate / numValidRecords);
                double avgTemp = round((sumTemp / numValidRecords) * 100) / 100.0;
                fprintf(outputFile, "1\n%.0f %.2f\n", avgDate, avgTemp);
            }   
        }
    }
    
 
    //close files
    fclose(inputFile);  
    fclose(outputFile);
}
//gcc -Wall -std=c11 Programs/Version-4/prog02_4.c -o prog02_4
// ./prog02_4 data4/input4.txt 1.0 1.0 data4/output4.txt
