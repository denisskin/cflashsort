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

#ifndef FLASH_SORT_NAME
#error "Must declare FLASH_SORT_NAME"
#endif

#ifndef FLASH_SORT_TYPE
#error "Must declare FLASH_SORT_TYPE"
#endif

#ifndef FLASH_SORT_KEY_SIZE
#define FLASH_SORT_KEY_SIZE sizeof(FLASH_SORT_TYPE)
#endif

#ifndef FLASH_SORT_GET_BYTE
#define FLASH_SORT_GET_BYTE(pValue, lv) (*(((unsigned char*)(pValue))+FLASH_SORT_KEY_SIZE-1-lv))
#endif

#ifndef FLASH_SORT_SWAP
#define FLASH_SORT_SWAP(x,y) { FLASH_SORT_TYPE __swap = *(x); *(x) = *(y); *(y) = __swap; }
#endif

#define FLASH_SORT_STACK_SIZE 100

void FLASH_SORT_NAME(FLASH_SORT_TYPE *values, size_t n) {

    typedef struct {
        FLASH_SORT_TYPE* pVal;
        unsigned len;
    } Bucket;

    typedef struct {
        int lv;
        FLASH_SORT_TYPE* pn;
    } Stack;

    if(n < 2) return;

    Bucket buckets[256];
    Bucket *b, *bp, *bLo, *bHi, *bMax = buckets + 256;
    memset(buckets, 0, 256 * sizeof(Bucket));

    FLASH_SORT_TYPE *p0 = values;
    FLASH_SORT_TYPE *pn;
    FLASH_SORT_TYPE *p;
    unsigned char c, c0, ch, ch0;
    Stack stack[FLASH_SORT_STACK_SIZE] = {{0, p0 + n}}, *pStack = stack;
    size_t lv;
    const size_t lastLv = FLASH_SORT_KEY_SIZE - 1;

start:
    lv = pStack->lv;    // level
    pn = pStack->pn;    // pointer to the end of array

    if(lv > lastLv) {
        // break iteration
        p0 = pn;
        pStack--;
        goto sub;
    }
    n = pn - p0;
    if(n < 8) { // use trivial sort
        if(n == 2) {  // sort 2 element
            for(p = p0 + 1; lv <= lastLv; lv++) {
                c0 = FLASH_SORT_GET_BYTE(p0, lv);
                c = FLASH_SORT_GET_BYTE(p, lv);
                if(c != c0) {
                    if(c0 > c) FLASH_SORT_SWAP(p, p0);
                    break;
                }
            }
        } else if(n > 2) { // use bubble sort
            for(FLASH_SORT_TYPE *pm = p0 + 1; pm < pn; pm++) {
                for(p = pm; p > p0; p--) {
                    char fSwap = 0;
                    for(size_t l = lv; l <= lastLv; l++) {
                        c0 = FLASH_SORT_GET_BYTE(p-1, l);
                        c = FLASH_SORT_GET_BYTE(p, l);
                        if(c0 != c) { fSwap = c0 > c; break; }
                    }
                    if(fSwap) { FLASH_SORT_SWAP(p, p-1); } else break;
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

    // 1. calc buckets sizes.
    for(p = p0; p < pn; p++) {
        //countBuckets += !(b = buckets + FLASH_SORT_GET_BYTE(p, lv))->len++;
        (b = buckets + FLASH_SORT_GET_BYTE(p, lv))->len++;
        if(b < bLo) bLo = b;
        if(b > bHi) bHi = b;
    }

    if(bLo == bHi) {  // if countBuckets == 1
        bLo->len = 0;
        pStack->lv++;
        goto start;
    }

    // 2. set scopes of buckets (pointers to value)
    if(lv == lastLv) { // last level
        for(b=bLo; b<=bHi; b++) {
            b->pVal = p0;
            p0 += b->len;
        }
        p0 = pn;

    } else {
        // skip buckets with length = 1
        for(b = bLo; b->len < 2 && b <= bHi; b++) {
            if(b->len) {
                b->pVal = p0++;
            }
        }
        if(p0 != pn) {
            // pre-set size of first sub.bucket for next iteration
            pStack++;
            pStack->pn = p0 + b->len;
            pStack->lv = lv + 1;
        }
        for(p = p0; b<=bHi; b++) {
            b->pVal = p;
            p += b->len;
        }
    }

    // 3. move values into appropriate buckets
    for(b = bLo; b < bHi; b++) {
        for(; b->len; b->len--, b->pVal++) {
            while((bp = buckets + FLASH_SORT_GET_BYTE(b->pVal, lv)) != b) {
                FLASH_SORT_SWAP(b->pVal, bp->pVal);
                bp->len--;
                bp->pVal++;
            }
        }
    }
    bHi->len = 0;

    if(p0 == pn) { // all buckets has length == 1 or last level. rise to up
        pStack--;
        goto sub;
    }

    // 4. start processing of sub buckets.  process first sub bucket
    goto start;

sub:
    // continue processing of sub buckets
    if(pStack < stack){
        return; // finish sorting
    }

    pn = pStack->pn;

    if(p0 >= pn) {
        pStack--;
        goto sub;
    }

    // calculate length of current sub-bucket
    lv = pStack->lv;
    ch = FLASH_SORT_GET_BYTE(p0, lv);
    for(p = p0 + 1; p < pn; p++) {
        if(ch != (ch0 = FLASH_SORT_GET_BYTE(p, lv))) break;
        ch = ch0;
    }

    pStack++;
    pStack->lv = lv+1;
    pStack->pn = p;
    goto start;
}


#undef FLASH_SORT_NAME
#undef FLASH_SORT_TYPE
#undef FLASH_SORT_KEY_SIZE
#undef FLASH_SORT_GET_BYTE
#undef FLASH_SORT_SWAP
#undef FLASH_SORT_STACK_SIZE
