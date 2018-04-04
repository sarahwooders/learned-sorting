
/***************************************************************************
*   Function   : RadixSort
*   Description: This function performs a single pass of a radix sort on
*                array of items.
*   Parameters : list - a pointer of an array of items to sort
*                numItems - number of items in the array
*                itemSize - size of each item in the array
*                numKeys - number of keys produced by keyFunc
*                keyFunc - a function that generates a key used for
*                          the current sorting pass.  The key values
*                          are expected to range from 0 to (numKeys - 1)
*   Effects    : The contents of list are sorted according to their key.
*                Multiple calls to this function may be required to obtain
*                the desired sort.
*   Returned   : NONE
***************************************************************************/
void RadixSort(void *list, size_t numItems, size_t itemSize,
    unsigned int numKeys, unsigned int (*keyFunc) (const void *))
{
    size_t *keyCounters;            /* num values with the same key */
    size_t *offsetTable;            /* position of next value with key */
    size_t i;
    unsigned int key;
    void *temp;

    /* create an array of zeroed key counters */
    keyCounters = (size_t *)calloc(numKeys, sizeof(size_t));
    assert(keyCounters != NULL);

    /* count occurances of values with same key */
    for (i = 0; i < numItems; i++)
    {
        key = keyFunc(VoidPtrOffset(list, (itemSize * i)));
        keyCounters[key] = keyCounters[key] + 1;
    }

    /* allocate offset table */
    offsetTable = (size_t *)malloc(numKeys * sizeof(size_t));
    assert(offsetTable != NULL);

    offsetTable[0] = 0;         /* the first key 0 item starts at 0 */

    for(i = 1; i < numKeys; i++)
    {
        /* determine sorted offset for the first value with key i */
        offsetTable[i] = offsetTable[i - 1] + keyCounters[i - 1];
    }

    free(keyCounters);          /* we're done with keyCounters now */

    temp = malloc(numItems * itemSize);
    assert(temp != NULL);

    /* now sort */
    for (i = 0; i < numItems; i++)
    {
        key = keyFunc(VoidPtrOffset(list, (itemSize * i)));

        /* copy list + (itemSize * i) into its sorted position */
        memcpy(VoidPtrOffset(temp, (offsetTable[key] * itemSize)),
            VoidPtrOffset(list, (itemSize * i)),
            itemSize);

        /* the next item with the same key is sorted one position higher */
        offsetTable[key] = offsetTable[key] + 1;
    }

    /* copy sorted data back to list */
    memcpy(list, temp, itemSize * numItems);

    free(offsetTable);
    free(temp);
}
