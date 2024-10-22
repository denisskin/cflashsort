#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../src/flashsort.h"

// define function flashsort_uint by macro
#define FLASH_SORT_NAME  flashsort_uint
#define FLASH_SORT_TYPE  uint32_t
#include "../src/flashsort_macro.h"

// benchmarks settings
#define MAX_VALUES          1000000
#define COUNT_STEPS         20   // count of points
#define COUNT_EXPERIMENTS   40   // count of experiments on one point

void benchmark_sort_str(const char* title, const char* filename);
void benchmark_sort_uint(const char* title, const char* filename);

int main() {

    benchmark_sort_uint("Sorting of IP-addresses",      "data/ip-addresses-as-nums.txt");
    benchmark_sort_uint("Sorting of integers",          "data/numbers.txt");

    benchmark_sort_str("Sorting of hashes base64",      "data/hashes-base64.txt");
    benchmark_sort_str("Sorting of hashes hex",         "data/hashes-hex.txt");
    benchmark_sort_str("Sorting of numbers as strings", "data/numbers.txt");
    benchmark_sort_str("Sorting of english words",      "data/words-en.txt");

    benchmark_sort_uint("Sorting of sorted integers",   "data/numbers-sorted.txt");
    benchmark_sort_uint("Sorting of r-sorted integers", "data/numbers-rsorted.txt");

    return 0;
}

static __inline int cmp_int(const void * a, const void * b) {
    return *(uint32_t*)a > *(uint32_t*)b? 1 : -1;
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
        printf("Error: can not open file %s!\n", filename);
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
    printf("\n---------------------------------------------------------------------------------------------");
    printf("\nCount           Flash-sort                   |   Quick-sort                   |          ");
    printf("\nelements        Total time                   |   Total time                   |          ");
    printf("\n      N             Tf            Tf / N     |       Tq            Tq / N     |      Δ  ");
    printf("\n---------------------------------------------------------------------------------------------");

    for(int step=1; step<=COUNT_STEPS; step++) {
        int n = (int)pow(nValues, 1. * step / COUNT_STEPS);

        clock_t t0, t;
        double st1=0, st2=0;

        printf("\n%7d      ", n);
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
        printf("   %lf sec     %.3lf µs    |", st1/COUNT_EXPERIMENTS, st1/COUNT_EXPERIMENTS/n*10e6);
        printf("   %lf sec     %.3lf µs    |", st2/COUNT_EXPERIMENTS, st2/COUNT_EXPERIMENTS/n*10e6);
        printf("   %+-7.2lf%%", (st2/st1-1)*100);    // difference in %

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
void benchmark_sort_uint(const char* title, const char* filename) {

    //------- read file ---------------
    int i, nValues = 0;
    FILE *fp = fopen(filename, "r");
    if(!fp) {
        printf("Error: can not open file %s!\n", filename);
        exit(1);
    }

    uint32_t *values  = (uint32_t*)malloc(MAX_VALUES * sizeof(uint32_t));
    uint32_t *values1 = (uint32_t*)malloc(MAX_VALUES * sizeof(uint32_t));
    uint32_t *values2 = (uint32_t*)malloc(MAX_VALUES * sizeof(uint32_t));
    if(!values || !values1 || !values2) {
        printf("ERROR: Can`t allocate memory!\n");
        exit(1);
    }
    for(nValues = 0; nValues<MAX_VALUES && !feof(fp); nValues++) {
        fscanf(fp, "%d", values+nValues);
    }

    printf("\n\n---------- benchmark %s %s -------------", title, filename);
    printf("\n total count values: %d", nValues);
    printf("\n---------------------------------------------------------------------------------------------");
    printf("\nCount           Flash-sort                   |   Quick-sort                   |          ");
    printf("\nelements        Total time                   |   Total time                   |          ");
    printf("\n      N             Tf            Tf / N     |       Tq            Tq / N     |      Δ  ");
    printf("\n---------------------------------------------------------------------------------------------");

    for(int step=1; step<=COUNT_STEPS; step++) {
        int n = (int)pow(nValues, 1. * step / COUNT_STEPS);

        clock_t t0, t;
        double st1=0, st2=0;

        printf("\n%7d      ", n);
        for(int k=0; k < COUNT_EXPERIMENTS; k++) { // count experiments
            // init data
            for(i = 0; i < n; i++) values2[i] = values1[i] = values[i];

            // -- flash sort
            t0 = clock();
            flashsort_uint(values1, n);
            //flashsort_const(values1, n, sizeof(uint32_t), sizeof(uint32_t));
            t = clock();
            st1 += (double)(t - t0) / CLOCKS_PER_SEC;

            // -- quick sort
            t0 = clock();
            qsort(values2, n, sizeof(int), cmp_int);
            t = clock();
            st2 += (double)(t - t0) / CLOCKS_PER_SEC;
        }
        // print avg time
        printf("   %lf sec     %.3lf µs    |", st1/COUNT_EXPERIMENTS, st1/COUNT_EXPERIMENTS/n*10e6);
        printf("   %lf sec     %.3lf µs    |", st2/COUNT_EXPERIMENTS, st2/COUNT_EXPERIMENTS/n*10e6);
        printf("   %+-7.2lf%%", (st2/st1-1)*100);    // difference in %

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
