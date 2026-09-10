#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void findminmax(float *array, int size, float *min, float *max){
    *min = array[0];
    *max = array[0];
    for(int i=0;i<size;i++){
        if (array[i]<*min){
            *min = array[i];
        }
        if (array[i]>*max){
            *max = array[i];
        }
    }
}
//two arguments: the path to an input data file and the path to the expected output file
int main(int argc, char* argv[]) {
    //storing file names
    const char *inputFilePath = argv[1];
    const char *outputFilePath = argv[2];


    FILE *file = fopen(inputFilePath, "r");
    //get the first line of the textfile
    int n;
    fscanf(file, "%d", &n);
    //allocate memory for two arrays, one for x values and one for y values
    float *x_array = (float *)malloc(n * sizeof(float));
    float *y_array = (float *)malloc(n * sizeof(float));
    for(int i=0; i<n; i++){
        fscanf(file, "%f %f",&x_array[i],&y_array[i]);
    }
    float xmin, xmax, ymin, ymax;
    findminmax(x_array,n, &xmin, &xmax);
    findminmax(y_array,n, &ymin, &ymax);
    //printf("xmin: %2f, xmax: %2f\n, ymin: %2f, ymax: %2f");
    xmin = floor(xmin * 10) / 10.0;
    ymin = floor(ymin * 10) / 10.0;
    xmax = ceil(xmax * 10) / 10.0;
    ymax = ceil(ymax * 10) / 10.0;
//output is  single text data file containing a single line listing the
//xmin , xmax , ymin , ymax values, in that order, separated by a blank character
    FILE *outputfile = fopen(outputFilePath, "w");
    fprintf(outputfile, "%.1f %.1f %.1f %.1f\n", xmin, xmax, ymin, ymax);
}


// compile gcc -o prog01v1 prog01v1.C
//Command line ex. ./prog01v1 dataFile1.txt Output/output1.txt