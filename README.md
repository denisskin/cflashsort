flashsort
=========

The algorithm is based on the method of bucket sort (bin sort).
The algorithm has a run time close to linear and uses O(1) memory.

Example
-------
``` c
int values[10] = [9, 6, 7, 0, 3];
flashsort(values, 10, sizeof(int), sizeof(int));
```

Benchmarks
----------
Benchmarks of sorting of strings.

run
``` shell
cd benchmarks
gcc ../src/*.c benchmarks_str.c  -o benchmarks.o && ./benchmarks.o
```
output
``` txt
-------------------------------------------------------------------------------------------------------
Count		Flash-sort		          	        Quick-sort
elements	total time	   one operation		  total time	 one operation
-------------------------------------------------------------------------------------------------------
    147		0.000087 sec	[5.913832 mcsec]	0.000061 sec	[4.160998 mcsec]	-29.64%
    274		0.000173 sec	[6.305353 mcsec]	0.000139 sec	[5.059611 mcsec]	-19.76%
    512		0.000292 sec	[5.705078 mcsec]	0.000335 sec	[6.540365 mcsec]	+14.64%
    955		0.000447 sec	[4.682024 mcsec]	0.000636 sec	[6.656894 mcsec]	+42.18%
   1782		0.000784 sec	[4.399364 mcsec]	0.001295 sec	[7.267864 mcsec]	+65.20%
   3326		0.001627 sec	[4.890559 mcsec]	0.002682 sec	[8.064843 mcsec]	+64.91%
   6208		0.003859 sec	[6.215475 mcsec]	0.005791 sec	[9.328179 mcsec]	+50.08%
  11585		0.008717 sec	[7.524011 mcsec]	0.011480 sec	[9.909682 mcsec]	+31.71%
  21618		0.017069 sec	[7.895858 mcsec]	0.022877 sec	[10.582293 mcsec]	+34.02%
  40342		0.028611 sec	[7.092104 mcsec]	0.046428 sec	[11.508668 mcsec]	+62.27%
  75281		0.048849 sec	[6.488941 mcsec]	0.090109 sec	[11.969620 mcsec]	+84.46%
 140479		0.096301 sec	[6.855205 mcsec]	0.196975 sec	[14.021669 mcsec]	+104.54%
```

Benchmarks of sorting of values constant length.

run
``` shell
cd benchmarks
gcc ../src/*.c benchmarks.c  -o benchmarks.o && ./benchmarks.o
```
output
``` txt
-------------------------------------------------------------------------------------------------------
Count		Flash-sort		          	        Quick-sort
elements	total time	   one operation		  total time	 one operation
-------------------------------------------------------------------------------------------------------
   1398		0.000223 sec	[1.594182 mcsec]	0.000155 sec	[1.112065 mcsec]	-30.24%
   2702		0.000608 sec	[2.250062 mcsec]	0.000409 sec	[1.512460 mcsec]	-32.78%
   5220		0.001338 sec	[2.563602 mcsec]	0.000807 sec	[1.545594 mcsec]	-39.71%
  10085		0.002714 sec	[2.691158 mcsec]	0.001666 sec	[1.651925 mcsec]	-38.62%
  19483		0.004321 sec	[2.217591 mcsec]	0.003061 sec	[1.571182 mcsec]	-29.15%
  37640		0.006318 sec	[1.678463 mcsec]	0.005989 sec	[1.591171 mcsec]	-5.20 %
  72716		0.010920 sec	[1.501769 mcsec]	0.012454 sec	[1.712626 mcsec]	+14.04%
 140479		0.019924 sec	[1.418288 mcsec]	0.025116 sec	[1.787878 mcsec]	+26.06%
 271388		0.039561 sec	[1.457722 mcsec]	0.052155 sec	[1.921800 mcsec]	+28.84%
 524288		0.087256 sec	[1.664272 mcsec]	0.105510 sec	[2.012436 mcsec]	+31.92%
```