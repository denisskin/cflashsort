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
#include <stdlib.h>

typedef unsigned char byte;

#define SWAP(pA, pB) { byte v, *a=(byte*)(pA), *b=(byte*)(pB); for(size_t nn=size;nn--;) v=*a, *a++=*b, *b++=v; }
#define BYTE(pValue, lv) *((byte*)(pValue)+lastLv-lv)

void flashsort_const(void *values, size_t n, size_t size, size_t size_key) {

    typedef struct {
        byte* pVal;
        unsigned len;
    } Bucket;

    typedef struct {
        int lv;
        byte* pn;
    } Stack;
    if(n < 2) return;

    Bucket buckets[256];
    Bucket *b, *bp, *bLo, *bHi, *bMax = buckets + 256;
    memset(buckets, 0, 256 * sizeof(Bucket));

    byte *p0 = (byte*)values, *pn, *p;
    Stack *pStack, *stack;
    byte c, c0, ch, ch0;
    size_t lv, lastLv = (size_key>0 && size_key<=size? size_key : size) - 1;
    size_t size2 = size + size;
    size_t size8 = size * 8;

    stack = pStack = malloc((size+1)*sizeof(Stack));
    stack->lv = 0;
    stack->pn = p0 + n*size;

start:
    lv = pStack->lv;    // level
    pn = pStack->pn;    // pointer to the end of array

    if(lv > lastLv) {
        // break iteration
        p0 = pn;
        pStack--;
        goto sub;
    }

    if(pn < p0 + size8) { // use trivial sort
        if(pn == p0 + size2) {  // sort 2 element
            p = p0 + size;
            // short sort
            for( ; lv <= lastLv; lv++) {
                c0 = BYTE(p0, lv);
                c = BYTE(p, lv);
                if(c != c0) {
                    if(c < c0) SWAP(p, p0);
                    break;
                }
            }
        } else if(pn > p0 + size2) { // use bubble sort
            for(byte *pm = p0 + size; pm < pn; pm+=size) {
                for(p = pm; p > p0; p-=size) {
                    char fSwap = 0;
                    for(size_t l = lv; l <= lastLv; l++) {
                        c0 = BYTE(p-size, l);
                        c = BYTE(p, l);
                        if(c0 != c) { fSwap = c0 > c; break; }
                    }
                    if(fSwap) { SWAP(p, p-size); } else break;
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
    for(p = p0; p < pn; p+=size) {
        //countBuckets += !(b = buckets + BYTE(p, lv))->len++;
        (b = buckets + BYTE(p, lv))->len++;
        if(b < bLo) bLo = b;
        if(b > bHi) bHi = b;
    }

    if(bLo == bHi) { // if countBuckets == 1
        bLo->len = 0;
        pStack->lv++;
        goto start;
    }

    // 2. set scopes of buckets (pointers to value)
    if(lv == lastLv) { // last level
        for(b=bLo; b<=bHi; b++) {
            b->pVal = p0;
            p0 += b->len * size;
        }
        p0 = pn;
    } else {
        // skip buckets with length = 1
        for(b = bLo; b->len < 2 && b <= bHi; b++) {
            if(b->len) {
                b->pVal = p0;
                p0 += size;
            }
        }
        if(p0 != pn) {
            // pre-set size of first sub.bucket for next iteration
            pStack++;
            pStack->pn = p0 + b->len * size;
            pStack->lv = lv+1;
        }
        for(p=p0; b<=bHi; b++) {
            b->pVal = p;
            p += b->len * size;
        }
    }

    // 3. move values into appropriate buckets
    for(b = bLo; b < bHi; b++) {
        for(; b->len; b->len--, b->pVal+=size) {
            while((bp = buckets + BYTE(b->pVal, lv)) != b) {
                SWAP(b->pVal, bp->pVal);
                bp->len--;
                bp->pVal+=size;
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
    if(pStack < stack) {
        // finish sorting
        free(stack);
        return;
    }

    pn = pStack->pn;

    if(p0 >= pn) {
        pStack--;
        goto sub;
    }

    // calculate length of current sub-bucket
    lv = pStack->lv;
    ch = BYTE(p0, lv);
    for(p = p0 + size; p < pn; p+=size) {
        if(ch != (ch0 = BYTE(p, lv))) break;
        ch = ch0;
    }

    pStack++;
    pStack->lv = lv+1;
    pStack->pn = p;
    goto start;
}


