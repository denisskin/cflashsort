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

#define BYTE(pValue, lv) get_byte(*pValue, lv)
#define SWAP(p1, p2) v=*p1, *p1=*p2, *p2=v

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

    pointer v, *p0 = values, *pn, *p;
    byte ch, ch0, countBuckets;
    const byte *c, *c0;
    size_t lv;
    Stack stack[STACK_SIZE] = {{0, p0 + n}}, *pStack = stack;

start:
    lv = pStack->lv;    // level
    pn = pStack->pn;    // pointer on end of array

    if(pn == p0 + 1) {  // only one element. rise up a level
        p0 = pn;
        pStack--;
        goto sub;
    }
    if(pn == p0 + 2) {  // compare 2 element and rise up a level
        // short sort
        for(p = p0 + 1; ; lv++) {
            if(!(c0 = BYTE(p0, lv))) break;
            if(!(c = BYTE(p, lv))) { SWAP(p, p0); break; }
            if(*c != *c0) {
                if(*c < *c0) SWAP(p, p0);
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

    // skip NULL values
    while(p0 < pn && !BYTE(p0, lv)) p0++;

    // calc buckets sizes.
    for(p = p0; p < pn; p++) {
        c = BYTE(p, lv);
        if(c) {
            countBuckets += !(b = buckets + *c)->len++;
            if(b < bLo) bLo = b;
            if(b > bHi) bHi = b;
        } else {
            // move NULL value to the beginning.  p0++
            SWAP(p, p0);
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

    // set scopes of buckets (pointers to value)

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

    // move values into appropriate buckets
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

    // start process sub buckets.  process first sub bucket
    goto start;

sub:
    // continue processing sub buckets
    if(pStack < stack) {
        return; // finish sort
    }
    lv = pStack->lv;
    pn = pStack->pn;

    if(p0 >= pn) {
        pStack--;
        goto sub;
    }

    // calculate length of current sub-bucket
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


