flashsort
=========

Fast stable in-place sorting with O(n) data moves and using O(1) memory.
The algorithm is based on the method of bucket sort (bin sort).

[Detail description](https://habrahabr.ru/post/280848/) of the algoritm (russian).


Example
-------
``` c
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

```

``` c
// sort integer values
int nums[10] = {9, 6, 7, 0, 3, 1, 3, 2, 5, 8};
flashsort_const(nums, 10, sizeof(int), sizeof(int));

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

flashsort_const(names, 10, sizeof(KeyValue), sizeof(names->key));

```

Define sort-function by C-macro

``` c
// define function flashsort_int by macro
#define FLASH_SORT_NAME  flashsort_int
#define FLASH_SORT_TYPE  int
#include "../src/flashsort_macro.h"

int nums[10] = { 9, 6, 7, 0, 3, 1, 3, 2, 5, 8 };
flashsort_int(nums, 10);

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
 
---------------------------------------------------------------------------------------------
Count           Flash-sort                   |   Quick-sort                   |
elements        Total time                   |   Total time                   |
      N             Tf            Tf / N     |       Tq            Tq / N     |      Δ
---------------------------------------------------------------------------------------------
    147         0.000009 sec     0.592 µs    |   0.000010 sec     0.655 µs    |   +10.63 %
    274         0.000020 sec     0.738 µs    |   0.000023 sec     0.829 µs    |   +12.36 %
    512         0.000052 sec     1.023 µs    |   0.000062 sec     1.209 µs    |   +18.13 %
    955         0.000090 sec     0.937 µs    |   0.000122 sec     1.282 µs    |   +36.78 %
   1782         0.000181 sec     1.015 µs    |   0.000306 sec     1.719 µs    |   +69.41 %
   3326         0.000307 sec     0.923 µs    |   0.000566 sec     1.702 µs    |   +84.43 %
   6208         0.000618 sec     0.995 µs    |   0.001199 sec     1.932 µs    |   +94.14 %
  11585         0.001306 sec     1.128 µs    |   0.002367 sec     2.043 µs    |   +81.21 %
  21618         0.002881 sec     1.333 µs    |   0.004623 sec     2.139 µs    |   +60.46 %
  40342         0.005950 sec     1.475 µs    |   0.009634 sec     2.388 µs    |   +61.91 %
  75281         0.010677 sec     1.418 µs    |   0.020008 sec     2.658 µs    |   +87.38 %
 140479         0.020686 sec     1.473 µs    |   0.040685 sec     2.896 µs    |   +96.68 %


![alt](https://habrastorage.org/files/455/e1e/8d9/455e1e8d94a54d6e9b7c88e785308fb5.png "Alt")


Benchmarks sorting of integers

---------------------------------------------------------------------------------------------
Count           Flash-sort                   |   Quick-sort                   |
elements        Total time                   |   Total time                   |
      N             Tf            Tf / N     |       Tq            Tq / N     |      Δ
---------------------------------------------------------------------------------------------
    100         0.000012 sec     1.225 µs    |   0.000004 sec     0.428 µs    |   -65.10 %
    194         0.000016 sec     0.814 µs    |   0.000009 sec     0.439 µs    |   -46.04 %
    374         0.000026 sec     0.690 µs    |   0.000023 sec     0.604 µs    |   -12.40 %
    724         0.000043 sec     0.592 µs    |   0.000058 sec     0.795 µs    |   +34.29 %
   1398         0.000099 sec     0.707 µs    |   0.000155 sec     1.112 µs    |   +57.28 %
   2702         0.000204 sec     0.753 µs    |   0.000300 sec     1.111 µs    |   +47.44 %
   5220         0.000410 sec     0.786 µs    |   0.000620 sec     1.187 µs    |   +51.02 %
  10085         0.000845 sec     0.838 µs    |   0.001254 sec     1.243 µs    |   +48.41 %
  19483         0.002205 sec     1.132 µs    |   0.002672 sec     1.372 µs    |   +21.21 %
  37640         0.004242 sec     1.127 µs    |   0.005436 sec     1.444 µs    |   +28.15 %
  72716         0.006886 sec     0.947 µs    |   0.011171 sec     1.536 µs    |   +62.23 %
 140479         0.011156 sec     0.794 µs    |   0.022899 sec     1.630 µs    |   +105.26%
 271388         0.018773 sec     0.692 µs    |   0.045749 sec     1.686 µs    |   +143.70%
 524288         0.037429 sec     0.714 µs    |   0.093858 sec     1.790 µs    |   +150.76%
 

Benchmarks sorting of IP-addresses log

---------------------------------------------------------------------------------------------
Count           Flash-sort                   |   Quick-sort                   |
elements        Total time                   |   Total time                   |
      N             Tf            Tf / N     |       Tq            Tq / N     |      Δ
---------------------------------------------------------------------------------------------
    107         0.000010 sec     0.953 µs    |   0.000011 sec     1.056 µs    |   +10.78 %
    208         0.000013 sec     0.639 µs    |   0.000010 sec     0.480 µs    |   -25.00 %
    407         0.000024 sec     0.584 µs    |   0.000033 sec     0.812 µs    |   +39.01 %
    793         0.000039 sec     0.493 µs    |   0.000073 sec     0.915 µs    |   +85.61 %
   1547         0.000080 sec     0.517 µs    |   0.000211 sec     1.367 µs    |   +164.15%
   3016         0.000169 sec     0.560 µs    |   0.000512 sec     1.697 µs    |   +202.80%
   5881         0.000308 sec     0.523 µs    |   0.000962 sec     1.636 µs    |   +212.67%
  11466         0.000548 sec     0.478 µs    |   0.001812 sec     1.581 µs    |   +230.48%
  22354         0.001034 sec     0.463 µs    |   0.004191 sec     1.875 µs    |   +305.26%
  43584         0.002109 sec     0.484 µs    |   0.008693 sec     1.994 µs    |   +312.26%
  84974         0.004411 sec     0.519 µs    |   0.016003 sec     1.883 µs    |   +262.81%
 165670         0.008837 sec     0.533 µs    |   0.037358 sec     2.255 µs    |   +322.75%
 323000         0.016046 sec     0.497 µs    |   0.081251 sec     2.516 µs    |   +406.36%
 629739         0.030895 sec     0.491 µs    |   0.164283 sec     2.609 µs    |   +431.75%
 
```
