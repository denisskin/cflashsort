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

#define SWAP(a, b)  swap((byte*)a, (byte*)b, size);
#define BYTE(pValue, lv) *((byte*)(pValue)+lastLv-lv)

static __inline void swap(byte *a, byte *b, size_t size){
    for(byte v; size--; ) {
        v = *a;
        *a++ = *b;
        *b++ = v;
    }
}

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
    size_t lv, lastLv = (size_key>0 && size_key<=size? size_key : size) - 1, size2 = size + size;

    stack = pStack = malloc((size+1)*sizeof(Stack));
    stack->lv = 0;
    stack->pn = p0 + n*size;

start:
    lv = pStack->lv;    // level
    pn = pStack->pn;    // pointer on end of array

    if(lv > lastLv) {
        p0 = pn;
        pStack--;
        goto sub;
    }

    if(pn == p0 + size) {  // only one element. rise up a level
        p0 = pn;
        pStack--;
        goto sub;
    }
    if(pn == p0 + size2) {  // compare 2 element and rise up a level
        p = p0 + size;
        // short sort
        for( ; ; lv++) {
            c0 = BYTE(p0, lv);
            c = BYTE(p, lv);
            if(c != c0) {
                if(c < c0) SWAP(p, p0);
                break;
            }
        }
        p0 = pn;
        pStack--;
        goto sub;
    }

    bHi = 0;
    bLo = bMax;
    //unsigned countBuckets = 0;

    // calc buckets sizes.
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

//    if(countBuckets == 2) {
//        pStack++;
//        pStack->lv = lv+1;
//        bLo->pVal = p0;
//        bHi->pVal = pStack->pn = p0 + bLo->len * size;
//        for(b = bLo; b->len; b->len--, b->pVal+=size) {
//            while((bp = buckets + BYTE(b->pVal, lv)) != b) {
//                SWAP(b->pVal, bp->pVal);
//                bp->len--;
//                bp->pVal+=size;
//            }
//        }
//        bHi->len = 0;
//        if(lv == lastLv) {
//            p0 = pn;
//            pStack -= 2;
//            goto sub;
//        }
//        goto start;
//    }

    // set scopes of buckets (pointers to value)
    if(lv == lastLv) { // last level
        for(b=bLo; b<=bHi; b++) {
            b->pVal = p0;
            p0 += b->len * size;
        }
        p0 = pn;
    } else {
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

    // move values into appropriate buckets
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

    // start process sub buckets.  process first sub bucket
    goto start;

sub:
    // continue processing sub buckets
    if(pStack < stack)
        goto finish;

    lv = pStack->lv;
    pn = pStack->pn;

    if(p0 >= pn) {
        pStack--;
        goto sub;
    }

    // calculate length of current sub-bucket
    ch = BYTE(p0, lv);
    for(p = p0 + size; p < pn; p+=size) {
        if(ch != (ch0 = BYTE(p, lv))) break;
        ch = ch0;
    }

    pStack++;
    pStack->lv = lv+1;
    pStack->pn = p;
    goto start;

finish:
    free(stack);
    return;
}


