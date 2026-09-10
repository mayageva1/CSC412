#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


//list of arguments: first a list of n ≥1 paths to input data files, and
//then the path to the expected output data file.
int main(int argc, char* argv[]) {
    //storing file names
    if (argc < 2) {
        return 1; // Indicate error
    }
    float xmin, xmax, ymin, ymax;
    bool first=true;
    for (int j=1; j<argc-1; j++){
        const char *inputFilePath = argv[j];
        FILE *file = fopen(inputFilePath, "r");
         if (file == NULL) {
            perror("Error opening file");
            continue; // Move to the next file
        }
        float x, y;
        //get the first line of the textfile
        int n;
        fscanf(file, "%d", &n);
        //allocate memory for two arrays, one for x values and one for y values
        //float *x_array = (float *)malloc(n * sizeof(float));
        //float *y_array = (float *)malloc(n * sizeof(float));
        for(int i=0; i<n; i++){
            fscanf(file, "%f %f",&x,&y);
            if (first){
                xmin = x;
                xmax = x;
                ymin = y;
                ymax = y;
                first=false;
            }
            else{
                if (x < xmin) xmin = x;
                if (x > xmax) xmax = x;
                if (y < ymin) ymin = y;
                if (y > ymax) ymax = y;
            }
        }  
        //printf("xmin: %2f, xmax: %2f\n, ymin: %2f, ymax: %2f");
    }
    xmin = floor(xmin * 10) / 10.0;
    ymin = floor(ymin * 10) / 10.0;
    xmax = ceil(xmax * 10) / 10.0;
    ymax = ceil(ymax * 10) / 10.0;
//output is  single text data file containing a single line listing the
//xmin , xmax , ymin , ymax values, in that order, separated by a blank character
    const char *outputFilePath = argv[argc-1];
    FILE *outputfile = fopen(outputFilePath, "w");
    fprintf(outputfile, "%.1f %.1f %.1f %.1f\n", xmin, xmax, ymin, ymax);
    fclose(outputfile);
}


// compile gcc -o prog01v1 prog01v1.C
//Command line ex. ./prog01v1 dataFile1.txt Output/output1.txt