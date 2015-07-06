#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../src/flashsort.h"

void benchmark(const char* filename);

int main() {
    benchmark("data/hashes-base64.txt");
    benchmark("data/hashes-hex.txt");
    benchmark("data/numbers.txt");
    benchmark("data/words-en.txt");
    return 0;
}

static int cmp_str(const void * a, const void * b) {
    return strcmp(*(char**)a, *(char**)b);
}

void benchmark(const char* filename) {

    //------- read file ---------------
    int i, nValues = 0;
    FILE *fp = fopen(filename, "r");
    if(!fp) {
        printf("Error: cant not open file %s!\n", filename);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long fileLen = ftell(fp);
    char *buf = malloc(fileLen+1), *buf0=buf;
    if(!buf) {
        printf("ERROR: Can`t allocate memory! (%ld bytes)\n", fileLen);
        exit(-1);
    }
    fseek(fp, 0, 0);
    while(!feof(fp)) nValues += !!fgets(buf, 100, fp);

    char **values  = (char**)malloc(nValues * sizeof(char*));
    char **values1 = (char**)malloc(nValues * sizeof(char*));
    char **values2 = (char**)malloc(nValues * sizeof(char*));
    if(!values) {
        printf("ERROR: Can`t allocate memory! (%ld bytes)\n", i * sizeof(char*));
        exit(1);
    }
    fseek(fp, 0, 0);
    for(i = 0; !feof(fp); i++) {
        values[i] = fgets(buf, 100, fp);
        int len = strlen(buf);
        buf[len-1] = 0;
        buf += len;
    }

    printf("\n\n------------ %s -------------", filename);
    printf("\n total count values: %d", nValues);
    printf("\n top-10 values: ");
    for(i=0; i<10; i++) printf(" %s,", values[i]);
    printf("... \n");
    printf("\n-------------------------------------------------------------------------------------------------------");
    printf("\nCount\t\tFlash-sort\t\t\tQuick-sort");
    printf("\nelements\ttotal time\tone operation\ttotal time\tone operation");
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
            flashsort_str(values1, n);
            t = clock();
            st1 += (double)(t - t0) / CLOCKS_PER_SEC;

            // -- quick sort
            t0 = clock();
            qsort(values2, n, sizeof(char*), cmp_str);
            t = clock();
            st2 += (double)(t - t0) / CLOCKS_PER_SEC;
        }
        // print avg time
        printf("\t%lf sec\t[%.3lf mcsec]", st1/K, st1/K/n*10e6);
        printf("\t%lf sec\t[%.3lf mcsec]", st2/K, st2/K/n*10e6);
        printf("\t%+-6.2lf%%", (st2/st1-1)*100);    // difference in %

        for(i = 0; i < n; i++) {
            if(strcmp(values1[i], values2[i])) {
                printf("\nNOT EQUAL %d (%s != %s)\n", i, values1[i], values2[i]);
                exit(1);
            }
        }
    }
    printf("\n\n top-10 sorted data: ");
    for(i=0; i<10; i++) printf(" %s,", values1[i]);
    printf("... \n");

    printf("\n\n");

    free(buf0);
    free(values);
    free(values1);
    free(values2);
}

