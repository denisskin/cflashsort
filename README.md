flashsort
=========

The algorithm is based on the method of bucket sort (bin sort).
The algorithm has a run time close to linear and uses O(1) memory.

Example
-------
``` c
// sort integer values
int nums[10] = {9, 6, 7, 0, 3, 1, 3, 2, 5, 8};
flashsort(nums, 10, sizeof(int), sizeof(int));
```

``` c
// sort key-value array by key
typedef struct {
    int  key;
    char *value;
} KeyValue;

KeyValue names[10] = {
    {8, "Hunter"},
    {9, "Isaac"},
    {3, "Christopher"},
    {2, "Bob"},
    {6, "Faith"},
    {1, "Alice"},
    {7, "Gabriel"},
    {4, "Denis"},
    {0, "none"},
    {5, "Ethan"},
};

flashsort(names, 10, sizeof(KeyValue), sizeof(names->key));


// sort array of strings
char *names[10] = {
    "Hunter",
    "Isaac",
    "Christopher",
    "Bob",
    "Faith",
    "Alice",
    "Gabriel",
    "Denis",
    "****",
    "Ethan",
};

static const char* get_char(const void *value, unsigned pos) {
    return *((char*)value + pos)? (char*)value + pos : NULL;
}

flashsort((void**)names, 10, get_char);

for(int i=0; i<10; i++) printf("\n\t%s", names[i]);

/* out:
    Alice
    Bob
    Christopher
    Denis
    Ethan
    Faith
    Gabriel
    Hunter
    Isaac
*/
```

Benchmarks
----------
Benchmarks of sorting of strings.

run
``` shell
cd benchmarks
gcc ../src/*.c benchmarks.c  -o benchmarks.o && ./benchmarks.o
```
output
``` txt

Benchmarks sorting of strings
 
--------------------------------------------------------------------------------------------
Count		Flash-sort		          	     Quick-sort
elements	total time	   one operation    total time	 one operation
--------------------------------------------------------------------------------------------
    147		0.000019 sec	[1.274 mcsec]	0.000010 sec	[0.710 mcsec]	-44.31%
    274		0.000038 sec	[1.393 mcsec]	0.000029 sec	[1.062 mcsec]	-23.76%
    512		0.000065 sec	[1.265 mcsec]	0.000058 sec	[1.141 mcsec]	-9.78 %
    955		0.000115 sec	[1.204 mcsec]	0.000142 sec	[1.492 mcsec]	+23.92%
   1782		0.000207 sec	[1.163 mcsec]	0.000303 sec	[1.702 mcsec]	+46.34%
   3326		0.000460 sec	[1.382 mcsec]	0.000671 sec	[2.017 mcsec]	+45.96%
   6208		0.001006 sec	[1.621 mcsec]	0.001316 sec	[2.121 mcsec]	+30.84%
  11585		0.002228 sec	[1.923 mcsec]	0.002676 sec	[2.310 mcsec]	+20.12%
  21618		0.004302 sec	[1.990 mcsec]	0.005269 sec	[2.437 mcsec]	+22.49%
  40342		0.007548 sec	[1.871 mcsec]	0.011148 sec	[2.763 mcsec]	+47.69%
  75281		0.013887 sec	[1.845 mcsec]	0.022099 sec	[2.936 mcsec]	+59.13%
 140479		0.027967 sec	[1.991 mcsec]	0.046108 sec	[3.282 mcsec]	+64.87% 


Benchmarks sorting of integers

-------------------------------------------------------------------------------------------
Count		Flash-sort		          	    Quick-sort
elements	total time	   one operation	total time	 one operation
-------------------------------------------------------------------------------------------
   1398		0.000223 sec	[1.594 mcsec]	0.000155 sec	[1.112 mcsec]	-30.24%
   2702		0.000608 sec	[2.250 mcsec]	0.000409 sec	[1.512 mcsec]	-32.78%
   5220		0.001338 sec	[2.563 mcsec]	0.000807 sec	[1.545 mcsec]	-39.71%
  10085		0.002714 sec	[2.691 mcsec]	0.001666 sec	[1.651 mcsec]	-38.62%
  19483		0.004321 sec	[2.217 mcsec]	0.003061 sec	[1.571 mcsec]	-29.15%
  37640		0.006318 sec	[1.678 mcsec]	0.005989 sec	[1.591 mcsec]	-5.20 %
  72716		0.010920 sec	[1.501 mcsec]	0.012454 sec	[1.712 mcsec]	+14.04%
 140479		0.019924 sec	[1.418 mcsec]	0.025116 sec	[1.787 mcsec]	+26.06%
 271388		0.039561 sec	[1.457 mcsec]	0.052155 sec	[1.921 mcsec]	+28.84%
 524288		0.087256 sec	[1.664 mcsec]	0.105510 sec	[2.012 mcsec]	+31.92%
```