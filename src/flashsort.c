//    The MIT License (MIT)
//
//    Copyright (c) 2015 Denis Glazkov
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//    SOFTWARE.
//
#include <string.h>

#define STACK_SIZE 1000

#define BYTE(value, lv) (*((char*)(value)+lv)? ((char*)(value)+lv) : 0)

#define SWAP(a, b)  v=b,b=a,a=v

typedef char* string;
typedef char byte;

typedef struct {
    string* pVal;
    unsigned len;
} Bucket;

typedef struct {
    int lv;
    string* pn;
} Stack;

void flashsort_str(char **values, size_t n) {
    if(n < 2) return;

    Bucket buckets[256];
    Bucket *b, *bp, *bLo, *bHi, *bMax = buckets + 256;
    memset(buckets, 0, 256 * sizeof(Bucket));

    Stack stack[STACK_SIZE] = {{0, values+n}}, *pStack = stack;
    string v, *p0 = values, *pn, *p;
    byte *c, *c0, ch, ch0;
    unsigned lv, countBuckets;

start:
    lv = pStack->lv;    // level
    pn = pStack->pn;    // pointer on end of array

    if(pn == p0 + 1) {  // one element. return
        p0 = pn;
        pStack--;
        goto sub;
    }
    if(pn == p0 + 2) {  // compare 2 element
        p = p0+1;
        // short short
        for( ; ; lv++) {
            if(!(c0 = BYTE(*p0, lv))) break;
            if(!(c = BYTE(*p, lv))) { SWAP(*p, *p0); break; }
            if(*c != *c0) {
                if(*c < *c0) SWAP(*p, *p0);
                break;
            }
        }
        p0 = pn;
        pStack--;
        goto sub;
    }

    bHi = 0;
    bLo = bMax;
    countBuckets = 0;

    // move pointer to not null value
    while(p0 < pn && !BYTE(*p0, lv)) p0++;

    // calc buckets lens
    for(p = p0; p < pn; p++) {
        c = BYTE(*p, lv);
        if(c) {
            b = buckets + *c;
            countBuckets += !b->len++;
            if(b < bLo) bLo = b;
            if(b > bHi) bHi = b;
        } else {
            // move NULL-byte value to begin.  p0++
            SWAP(*p, *p0);
            p0++;
        }
    }

    if(countBuckets == 0) {
        pStack--;
        goto sub;
    }
    
    if(countBuckets == 1) {
        bLo->len = 0;
        pStack->lv++;
        goto start;
    }
    
    if(countBuckets == 2) {
        bLo->pVal = p0;
        bHi->pVal = pStack->pn = p0 + bLo->len;
        for(b = bLo; b->len; b->len--, b->pVal++) {
            while((bp = buckets + *BYTE(*b->pVal, lv)) != b) {
                SWAP(*b->pVal, *bp->pVal);
                bp->len--;
                bp->pVal++;
            }
        }
        bHi->len = 0;
        pStack++;
        pStack->lv = lv+1;
        goto start;
    }

    // todo: move p0 and bLo for all first buckets with length == 1
    // set scopes of buckets (pointers to value)
    for(p=p0, b=bLo; b<=bHi; b++) {
        b->pVal = p;
        p += b->len;
    }
    
    // pre-set size of first bucket for sub iteration
    pStack++;
    pStack->pn = p0 + bLo->len;
    pStack->lv = lv+1;

    // move values into appropriate buckets
    for(b = bLo; b < bHi; b++) {
        for(; b->len; b->len--, b->pVal++) {
            while((bp = buckets + *BYTE(*b->pVal, lv)) != b) {
                SWAP(*b->pVal, *bp->pVal);
                bp->len--;
                bp->pVal++;
            }
        }
    }
    bHi->len = 0;
    goto start;

sub:
    // process sub buckets

    if(pStack < stack) return; // finish sort
    lv = pStack->lv;
    pn = pStack->pn;

    if(p0 >= pn) {
        pStack--;
        goto sub;
    }

    // calculate length of current sub-bucket
    ch = *BYTE(*p0, lv);
    for(p = p0 + 1; p < pn; p++) {
        if(ch != (ch0 = *BYTE(*p, lv))) break;
        ch = ch0;
    }

    pStack++;
    // todo: check stack size
//    if(pStack > stack + STACK_SIZE) {
//        printf("\n\n STACK OVERFLOW \n");
//        return;
//    }
    pStack->lv = lv+1;
    pStack->pn = p;
    goto start;
}


