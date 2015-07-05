#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../src/flashsort.h"

void benchmark(const char* filename);

int main() {
    benchmark("data/numbers2.txt");
    return 0;
}

typedef int type;
#define FMT "%d"
#define MAX_VALUES 1000000

static int cmp(const void * a, const void * b) {
    return *(type*)a > *(type*)b? 1 : -1;
}

void benchmark(const char* filename) {

    //------- read file ---------------
    int i, nValues = 0;
    FILE *fp = fopen(filename, "r");
    if(!fp) {
        printf("Error: cant not open file %s!\n", filename);
        exit(1);
    }

    type*values  = (type*)malloc(MAX_VALUES * sizeof(type));
    type*values1 = (type*)malloc(MAX_VALUES * sizeof(type));
    type*values2 = (type*)malloc(MAX_VALUES * sizeof(type));
    if(!values || !values1 || !values2) {
        printf("ERROR: Can`t allocate memory!\n");
        exit(1);
    }
    for(nValues = 0; nValues<MAX_VALUES && !feof(fp); nValues++) {
        fscanf(fp, FMT, values+nValues);
    }

    printf("\n\n------------ %s -------------", filename);
    printf("\n total count values: %d", nValues);
    printf("\n top-10 values: ");
    for(i=0; i<10; i++) printf(" "FMT",", values[i]);
    printf("... \n");
    printf("\n-------------------------------------------------------------------------------------------------------");
    printf("\nCount\t\tFlash-sort\t\t          \tQuick-sort");
    printf("\nelements\ttotal time\tone operation\t\ttotal time\tone operation");
    printf("\n-------------------------------------------------------------------------------------------------------");

    const int STEPS = 20;   // count of points
    const int K     = 30;   // count of experiments on one point

    for(int step=1; step<=STEPS; step++) {
        int n = (int)pow(nValues, 1. * step / STEPS);

        clock_t t0, t;
        double st1=0, st2=0;

        printf("\n%7d\t", n);
        for(int k=0; k < K; k++) { // count experiments
            // init data
            for(i = 0; i < n; i++) values2[i] = values1[i] = values[i];

            // -- flash sort
            t0 = clock();
            flashsort(values1, n, sizeof(type));
            t = clock();
            st1 += (double)(t - t0) / CLOCKS_PER_SEC;

            // -- quick sort
            t0 = clock();
            qsort(values2, n, sizeof(type), cmp);
            t = clock();
            st2 += (double)(t - t0) / CLOCKS_PER_SEC;
        }
        // print avg time
        printf("\t%lf sec\t[%lf mcsec]", st1/K, st1/K/n*10e6);
        printf("\t%lf sec\t[%lf mcsec]", st2/K, st2/K/n*10e6);
        printf("\t%+-6.2lf%%", (st2/st1-1)*100);    // difference in %

        for(i = 0; i < n; i++) {
            if(values1[i] != values2[i]) {
                printf("\nNOT EQUAL %d ("FMT" != "FMT")\n", i, values1[i], values2[i]);
                exit(1);
            }
        }
    }
    printf("\n\n top-10 sorted data: ");
    for(i=0; i<10; i++) printf(" "FMT",", values1[i]);
    printf("... \n");

    printf("\n\n");

    free(values);
    free(values1);
    free(values2);
}

