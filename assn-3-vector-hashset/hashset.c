#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
    assert((elemSize > 0) && (numBuckets > 0) && (hashfn != NULL) && (comparefn != NULL));
    h->elemSize = elemSize;
    h->numBuckets = numBuckets;
    h->hashfn = hashfn;
    h->compfn = comparefn;
    h->freefn = freefn;
    h->elems = malloc(numBuckets * elemSize);
   // VectorNew(&h->elems, elemSize, 
}

void HashSetDispose(hashset *h)
{
    if (h->freefn == NULL) {
    } else {
	h->freefn(h->elems);
    }
}

int HashSetCount(const hashset *h)
{ 
    return 0; 
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
    int i;
    void *elem;
    for (i = 0; i < h->numBuckets; i++) {
	elem = (char *)h->elems + i * h->elemSize;
	mapfn(elem, auxData);
    }
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
    void *element = h->elems;
    int index = h->hashfn(elemAddr, h->numBuckets);
    element = (char *)element + index * h->elemSize;
    memcpy(element, elemAddr, h->elemSize);
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
    void *element = h->elems;
    int i;
    for (i = 0; i < h->numBuckets; i++) {
	if (h->compfn(element, elemAddr) == 0) {    // if characters match in example
	    return element;
	}
	element = (char *)element + h->elemSize;
    }
    return NULL; 
}
