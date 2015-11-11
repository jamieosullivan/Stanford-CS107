#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    assert(elemSize > 0 && initialAllocation > 0);
    v->elemsize = elemSize;
    v->alloclen = initialAllocation;
    v->allocinc = initialAllocation;
    v->freefn = freeFn;
    v->loglen = 0;
    // TODO Initialize the allocated memory
    v->elems = malloc(elemSize * initialAllocation);
    assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
    if (v->freefn == NULL) { // simple free
	free(v->elems);
    } else {		    // need to invoke the free function for each element
	void *elemptr = (void *)v->elems;
	int index = 0;
	while (index < v->loglen) {
	    //elemptr = (char *)elemptr + index * v->elemsize;
	    v->freefn(elemptr);
	    elemptr = (char *)elemptr + v->elemsize;
	    index++;
	}
    }
    // Re-initialize loglen etc.?
}

int VectorLength(const vector *v)
{ 
    return v->loglen; 
}

void *VectorNth(const vector *v, int position)
{ 
    void *ret;
    assert((position >= 0) && (position <= (v->loglen-1)));
    ret = (char *)v->elems + position * v->elemsize;
    return ret; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert((position >= 0) && (position <= (v->loglen-1)));
    void *pos = (char *)v->elems + position * v->elemsize;
    if (v->freefn == NULL) {

    } else {
	v->freefn(pos);
    }
    memcpy(pos, elemAddr, v->elemsize);

}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert((position >= 0) && (position <= (v->loglen)));
    void *pos = (char *)v->elems + position * v->elemsize;
    void *dst = (char *)pos + v->elemsize;
    int len = (v->loglen - position) * v->elemsize; // loglen - 1 ??
    if (v->loglen == v->alloclen) {
	v->elems = realloc(v->elems, v->alloclen + v->allocinc * v->elemsize);
	v->alloclen += v->allocinc;
    } 
    memmove(dst, pos, len);
    memcpy(pos, elemAddr, v->elemsize);
    v->loglen += 1;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    if (v->loglen == v->alloclen) {
	// Add more elements
	// TODO initialise the new part of the vector
	//v->elems = realloc(v->elems, v->alloclen + v->allocinc * v->elemsize);
	v->elems = realloc(v->elems, (v->alloclen + v->allocinc) * v->elemsize);
	v->alloclen += v->allocinc;
	void *dst = (char *)v->elems + v->loglen * v->elemsize;
	memcpy(dst, elemAddr, v->elemsize);
	v->loglen += 1;
    } else {	// could remove this duplication
	void *dst = (char *)v->elems + v->loglen * v->elemsize;
	memcpy(dst, elemAddr, v->elemsize); 
	v->loglen += 1;
    }
}

void VectorDelete(vector *v, int position)
{
    assert((position >= 0) && (position <= (v->loglen-1)));
    void *pos = (char *)v->elems + position * v->elemsize;
    void *src = (char *)pos + v->elemsize;
    int len = (v->loglen - position - 1) * v->elemsize;
    if (v->freefn == NULL) {
    } else {
	v->freefn(pos);
    }
    memmove(pos, src, len);
    v->loglen--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    // Use qsort
    // void qsort(void *base, size_t nmemb, size_t size,
    //                   int (*compar)(const void *, const void *));
    qsort(v->elems, v->loglen, v->elemsize, compare);

}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    void *elemptr = (void *)v->elems;
    int index = 0;
    while (index < v->loglen) {
	elemptr = (char *)v->elems + index * v->elemsize;
	mapFn(elemptr, auxData);
	index++;
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
    void *res;
    int foundIndex;
    assert((startIndex >= 0) && (startIndex <= v->loglen));
    assert((key != NULL) && (searchFn != NULL));

    // Check value of isSorted to decide whether to use bisection or linear search
    if (isSorted) {
	// Use binary search
//	res = (int *)bsearch(key, v->elems, v->loglen, v->elemsize, searchFn);
	res = bsearch(key, v->elems, v->loglen, v->elemsize, searchFn);
	if (res == NULL) {
//	    printf("Not found\n");
	    return -1;
	} else {
	    foundIndex = ((char *)res - (char *)v->elems) / v->elemsize;
//	    printf("Found %c at index %d\n", *(char *)res, foundIndex);
	    return foundIndex;
	}
    } else {
	// Use linear search
	res = lfind(key, v->elems, (size_t *)&v->loglen, v->elemsize, searchFn);
	if (res == NULL) {
//	    printf("Not found\n");
	    return -1;
	} else {
	    foundIndex = ((char *)res - (char *)v->elems) / v->elemsize;
//	    printf("Found %c at index %d\n", *(char *)res, foundIndex);
	    return foundIndex;
	}
    }

    return -1; 
} 
