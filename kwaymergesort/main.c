/* ============================================================================
 *  main.c (k-way mergesort) 
 * ============================================================================
 *  Irene Finocchi (finocchi@di.uniroma1.it)
 */

#include <stdio.h>
#include <stdlib.h>
#include "multiwayMergesort.h"
#include <assert.h>
//HEAP TEST _ TODO REMOVE
#include "heap_ib/bheap.h"

/* type of file items */
typedef unsigned long ItemType;

// comparator for ItemType integers

int compar(const void* a, const void* b) {
    return *(ItemType*) a - *(ItemType*) b;
}

int main(int argc, char** argv) {

    int theErrCode;
    /* check command line parameters */
    if (argc < 5)
        exit((fprintf(stderr,
            "Usage: %s fileName block_size_in_bytes k internal/external(0/1)\nMain memory size M = blockSize x k\n", argv[0]), 1));

    /* sort file of ItemType integers */
    theErrCode = sort(argv[1], sizeof (ItemType), compar, atol(argv[2]), atol(argv[3]), atol(argv[4]), 1);

    /* check if the call failed */
    if (theErrCode != 0)
        fprintf(stderr, "%s\n", GetErrorString(theErrCode));

    return theErrCode;
}

////HEAP TEST _ TODO REMOVE
//#define MIN_HEAP	
//
//int int_compare(const void* left, const void* right) {
//    const int* p_left = left;
//    const int* p_right = right;
//
//#ifdef MAX_HEAP	
//    return (*p_left) - (*p_right); // max-heap
//#else
//    return (*p_right) - (*p_left); // min-heap
//#endif
//}
//
//boolean heap_elem_destroy(void *data) {
//    return TRUE;
//}
//
//// main function
//
//int main(int argc, char** argv) {
//    //HEAP TEST _ TODO REMOVE
//    bheap_t heap;
//
//    bheap_create(&heap, sizeof (int), 1, int_compare, heap_elem_destroy);
//
//    int num = 5;
//    bheap_push(&heap, &num);
//
//    num = 2;
//    bheap_push(&heap, &num);
//
//
//    int* p_num = bheap_peek(&heap);
//    printf("c%d\n", *p_num);
//    
//
//    bheap_pop(&heap);
//
//    p_num = bheap_peek(&heap);
//    printf( "c%d\n", *p_num);
//
//    
//    return 0;
//}
