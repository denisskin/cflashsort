#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../src/flashsort.h"

#define MAX_VALUES          1000000
#define COUNT_STEPS         20   // count of points
#define COUNT_EXPERIMENTS   40   // count of experiments on one point

void benchmark_sort_str(const char* title, const char* filename);
void benchmark_sort_int(const char* title, const char* filename);

// define function flashsort_int by macro
#define FLASH_SORT_NAME  flashsort_int
#define FLASH_SORT_TYPE  int
#include "../src/flashsort_macro.h"


int main() {

    benchmark_sort_str("Sorting of hashes base64",      "data/hashes-base64.txt");
    benchmark_sort_str("Sorting of hashes hex",         "data/hashes-hex.txt");
    benchmark_sort_str("Sorting of numbers as strings", "data/numbers.txt");
    benchmark_sort_str("Sorting of english words",      "data/words-en.txt");

    benchmark_sort_int("Sorting of integers",           "data/numbers.txt");
    benchmark_sort_int("Sorting of sorted integers",    "data/numbers-sorted.txt");
    benchmark_sort_int("Sorting of r-sorted integers",  "data/numbers-rsorted.txt");

    return 0;
}

static __inline int cmp_int(const void * a, const void * b) {
    return *(int*)a > *(int*)b? 1 : -1;
}

static __inline int cmp_str(const void * a, const void * b) {
    return strcmp(*(char**)a, *(char**)b);
}

static __inline const unsigned char* get_char(const void *value, unsigned pos) {
    return *((unsigned char*)value + pos)? (unsigned char*)value + pos : NULL;
}

//------------ sort strings -----------------
void benchmark_sort_str(const char* title, const char* filename) {

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
    if(!values || !values1 || !values2) {
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

    printf("\n\n---------- benchmark %s\t%s -------------", title, filename);
    printf("\n total count values: %d", nValues);
    printf("\n-------------------------------------------------------------------------------------------------------");
    printf("\nCount\t\tFlash-sort\t\t\tQuick-sort");
    printf("\nelements\ttotal time\tone operation\ttotal time\tone operation");
    printf("\n-------------------------------------------------------------------------------------------------------");

    for(int step=1; step<=COUNT_STEPS; step++) {
        int n = (int)pow(nValues, 1. * step / COUNT_STEPS);

        clock_t t0, t;
        double st1=0, st2=0;

        printf("\n%7d\t", n);
        for(int k=0; k < COUNT_EXPERIMENTS; k++) { // count experiments
            // init data
            for(i = 0; i < n; i++) values2[i] = values1[i] = values[i];

            // -- flash sort
            t0 = clock();
            flashsort((void**)values1, n, get_char);
            t = clock();
            st1 += (double)(t - t0) / CLOCKS_PER_SEC;

            // -- quick sort
            t0 = clock();
            qsort(values2, n, sizeof(char*), cmp_str);
            t = clock();
            st2 += (double)(t - t0) / CLOCKS_PER_SEC;
        }
        // print avg time
        printf("\t%lf sec\t[%.3lf mcsec]", st1/COUNT_EXPERIMENTS, st1/COUNT_EXPERIMENTS/n*10e6);
        printf("\t%lf sec\t[%.3lf mcsec]", st2/COUNT_EXPERIMENTS, st2/COUNT_EXPERIMENTS/n*10e6);
        printf("\t%+-6.2lf%%", (st2/st1-1)*100);    // difference in %

        for(i = 0; i < n; i++) {
            if(strcmp(values1[i], values2[i])) {
                printf("\nNOT EQUAL %d (%s != %s)\n", i, values1[i], values2[i]);
                exit(1);
            }
        }
    }
    printf("\n\n");

    free(buf0);
    free(values);
    free(values1);
    free(values2);
}

//------------ sort integers -----------------
void benchmark_sort_int(const char* title, const char* filename) {

    //------- read file ---------------
    int i, nValues = 0;
    FILE *fp = fopen(filename, "r");
    if(!fp) {
        printf("Error: cant not open file %s!\n", filename);
        exit(1);
    }

    int*values  = (int*)malloc(MAX_VALUES * sizeof(int));
    int*values1 = (int*)malloc(MAX_VALUES * sizeof(int));
    int*values2 = (int*)malloc(MAX_VALUES * sizeof(int));
    if(!values || !values1 || !values2) {
        printf("ERROR: Can`t allocate memory!\n");
        exit(1);
    }
    for(nValues = 0; nValues<MAX_VALUES && !feof(fp); nValues++) {
        fscanf(fp, "%d", values+nValues);
    }

    printf("\n\n---------- benchmark %s %s -------------", title, filename);
    printf("\n total count values: %d", nValues);
    printf("\n-------------------------------------------------------------------------------------------------------");
    printf("\nCount\t\tFlash-sort\t\t\tQuick-sort");
    printf("\nelements\ttotal time\tone operation\ttotal time\tone operation");
    printf("\n-------------------------------------------------------------------------------------------------------");

    for(int step=1; step<=COUNT_STEPS; step++) {
        int n = (int)pow(nValues, 1. * step / COUNT_STEPS);

        clock_t t0, t;
        double st1=0, st2=0;

        printf("\n%7d\t", n);
        for(int k=0; k < COUNT_EXPERIMENTS; k++) { // count experiments
            // init data
            for(i = 0; i < n; i++) values2[i] = values1[i] = values[i];

            // -- flash sort
            t0 = clock();
            flashsort_int(values1, n);
            //flashsort_const(values1, n, sizeof(int), sizeof(int));
            t = clock();
            st1 += (double)(t - t0) / CLOCKS_PER_SEC;

            // -- quick sort
            t0 = clock();
            qsort(values2, n, sizeof(int), cmp_int);
            t = clock();
            st2 += (double)(t - t0) / CLOCKS_PER_SEC;
        }
        // print avg time
        printf("\t%lf sec\t[%.3lf mcsec]", st1/COUNT_EXPERIMENTS, st1/COUNT_EXPERIMENTS/n*10e6);
        printf("\t%lf sec\t[%.3lf mcsec]", st2/COUNT_EXPERIMENTS, st2/COUNT_EXPERIMENTS/n*10e6);
        printf("\t%+-6.2lf%%", (st2/st1-1)*100);    // difference in %

        for(i = 0; i < n; i++) {
            if(values1[i] != values2[i]) {
                printf("\nNOT EQUAL %d (%d != %d)\n", i, values1[i], values2[i]);
                exit(1);
            }
        }
    }

    printf("\n\n");

    free(values);
    free(values1);
    free(values2);
}
