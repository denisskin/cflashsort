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

#define STACK_SIZE 100

typedef char byte;
typedef void* pointer;

#define BYTE(pValue, lv) get_byte(*(pValue), lv)
#define SWAP(p1, p2) { pointer __swap=*(p1); *(p1)=*(p2); *(p2)=__swap; }

void flashsort(void **values, size_t n, const char* get_byte(const void*, unsigned int)) {

    typedef struct {
        unsigned len;
        pointer* pVal;
    } Bucket;

    typedef struct {
        int lv;
        pointer* pn;
    } Stack;
    if(n < 2) return;

    Bucket buckets[256], *b, *bp, *bLo, *bHi, *bMax = buckets + 256;
    memset(buckets, 0, 256 * sizeof(Bucket));

    pointer *p0 = values, *pn, *p;
    byte ch, ch0;
    const byte *c, *c0;
    size_t lv;
    Stack stack[STACK_SIZE] = {{0, p0 + n}}, *pStack = stack;

start:
    lv = pStack->lv;    // level
    pn = pStack->pn;    // pointer on end of array

    n = pn - p0;
    if(n < 8) { // use trivial sort
        if(n == 2) {  // sort 2 element
            for(p = p0 + 1; ; lv++) {
                if(!(c0 = BYTE(p0, lv))) break;
                if(!(c = BYTE(p, lv))) { SWAP(p, p0); break; }
                if(*c != *c0) {
                    if(*c0 > *c) SWAP(p, p0);
                    break;
                }
            }
        } else if(n > 2) { // use bubble sort
            for(pointer *pm = p0 + 1; pm < pn; pm++) {
                for(p = pm; p > p0; p--) {
                    char fSwap = 0;
                    for(size_t l = 0; ; l++) {
                        if(!(c0 = BYTE(p-1, l))) { break; }
                        if(!(c = BYTE(p, l))) { fSwap = 1; break; }
                        if(*c0 != *c) { fSwap = *c0 > *c; break; }
                    }
                    if(fSwap) { SWAP(p, p-1); } else break;
                }
            }
        }
        // break iteration
        p0 = pn;
        pStack--;
        goto sub;
    }

    bHi = 0;
    bLo = bMax;
    //unsigned countBuckets = 0;

    // skip NULL values
    while(p0 < pn && !BYTE(p0, lv)) p0++;

    // 1. calc buckets sizes
    for(p = p0; p < pn; p++) {
        c = BYTE(p, lv);
        if(c) {
            //countBuckets += !(b = buckets + *c)->len++;
            (b = buckets + *c)->len++;
            if(b < bLo) bLo = b;
            if(b > bHi) bHi = b;
        } else {
            // move NULL value to the beginning.  p0++
            SWAP(p, p0);
            p0++;
        }
    }

    if(!bHi) { // if countBuckets == 0
        pStack--;
        goto sub;
    }

    if(bLo == bHi) { // if countBuckets == 1
        bLo->len = 0;
        pStack->lv++;
        goto start;
    }

//    if(countBuckets == 2) {
//        pStack++;
//        pStack->lv = lv+1;
//        bLo->pVal = p0;
//        bHi->pVal = pStack->pn = p0 + bLo->len;
//        for(b = bLo; b->len; b->len--, b->pVal++) {
//            while((bp = buckets + *BYTE(b->pVal, lv)) != b) {
//                SWAP(b->pVal, bp->pVal);
//                bp->len--;
//                bp->pVal++;
//            }
//        }
//        bHi->len = 0;
//        goto start;
//    }

    // 2. set scopes of buckets (pointers to value)

    // skip NULL values and values with length = 1
    for(b = bLo; b->len < 2 && b <= bHi; b++) {
        if(b->len) {
            b->pVal = p0;
            p0++;
        }
    }

    if(p0 != pn) {
        // pre-set size of first sub.bucket for next iteration
        pStack++;
        pStack->pn = p0 + b->len;
        pStack->lv = lv + 1;
    }

    for(p = p0; b <= bHi; b++) {
        b->pVal = p;
        p += b->len;
    }

    // 3. move values into appropriate buckets
    for(b = bLo; b < bHi; b++) {
        for(; b->len; b->len--, b->pVal++) {
            while((bp = buckets + *BYTE(b->pVal, lv)) != b) {
                SWAP(b->pVal, bp->pVal);
                bp->len--;
                bp->pVal++;
            }
        }
    }
    bHi->len = 0;

    if(p0 == pn) { // all buckets has length == 1. rise to up
        pStack--;
        goto sub;
    }

    // 4. start processing of sub buckets.  process first sub bucket
    goto start;

sub:
    // continue processing sub buckets
    if(pStack < stack) {
        // finish sorting
        return;
    }
    pn = pStack->pn;

    if(p0 >= pn) {
        pStack--;
        goto sub;
    }

    // calculate length of current sub-bucket
    lv = pStack->lv;
    ch = *BYTE(p0, lv);
    for(p = p0 + 1; p < pn; p++) {
        if(ch != (ch0 = *BYTE(p, lv))) break;
        ch = ch0;
    }

    pStack++;
    pStack->lv = lv+1;
    pStack->pn = p;
    goto start;
}


