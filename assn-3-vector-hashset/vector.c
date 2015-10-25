#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
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
	free(v);
    } else {		    // need to invoke the free function for each element
	void *elemptr = (void *)v->elems;
	int index = 0;
	while (index < v->loglen) {
	    elemptr = (char *)elemptr + index * v->elemsize;
	    v->freefn(elemptr);
	    index++;
	}
    }
}

int VectorLength(const vector *v)
{ return 0; }

void *VectorNth(const vector *v, int position)
{ return NULL; }

void VectorReplace(vector *v, const void *elemAddr, int position)
{}

void VectorInsert(vector *v, const void *elemAddr, int position)
{}

void VectorAppend(vector *v, const void *elemAddr)
{
    if (v->loglen == v->alloclen) {
	// Add more elements
	// TODO initialise the new part of the vector
	v->elems = realloc(v->elems, v->alloclen + v->allocinc * v->elemsize);
	v->alloclen += v->allocinc;
	void *dst = (char *)v->elems + v->loglen * v->elemsize;
	memcpy(dst, elemAddr, v->elemsize);
	v->loglen += 1;
    } else {
	void *dst = (char *)v->elems + v->loglen * v->elemsize;
	memcpy(dst, elemAddr, v->elemsize); 
	v->loglen += 1;
    }
}

void VectorDelete(vector *v, int position)
{}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    // Use qsort
    // void qsort(void *base, size_t nmemb, size_t size,
    //                   int (*compar)(const void *, const void *));

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
{ return -1; } 
