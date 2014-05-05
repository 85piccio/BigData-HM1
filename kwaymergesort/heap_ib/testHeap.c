/////*
//// * Copyright (C) 2010 by Joseph A. Marrero and Shrewd LLC. http://www.manvscode.com/
//// * 
//// * Permission is hereby granted, free of charge, to any person obtaining a copy
//// * of this software and associated documentation files (the "Software"), to deal
//// * in the Software without restriction, including without limitation the rights
//// * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//// * copies of the Software, and to permit persons to whom the Software is
//// * furnished to do so, subject to the following conditions:
//// * 
//// * The above copyright notice and this permission notice shall be included in
//// * all copies or substantial portions of the Software.
//// * 
//// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//// * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//// * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//// * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//// * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//// * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//// * THE SOFTWARE.
//// */
////#include <stdlib.h>
////#include <stdio.h>
////#include <time.h>
////#include "../heap_ib/bheap.h"
////
////#define SIZE  		40
////
//////#define MAX_HEAP	
////
////static int     int_compare( const void* left, const void* right );
////static boolean heap_elem_destroy( void *data );
////
////
////int main( int argc, char *argv[] )
////{
////	bheap_t heap;
////	int i;
////
////	time_t t = time(NULL);
////
////	srand( t );
////	bheap_create( &heap, sizeof(int), 1, int_compare, heap_elem_destroy );
////
////	printf( "seed = %ld\n", t );
////
////	for( i = 0; i < SIZE; i++ )
////	{
////		int num = rand() % 44;
////		bheap_push( &heap, &num );
////		//printf( "Pushed %d\n", num );
////	}
////
////	printf( "   ----- Sorted Output -----\n" );
////	for( i = 0; i < SIZE; i++ )
////	{
////		int* p_num = bheap_peek( &heap );
////		printf( "%10d (size = %02ld) \n", *p_num, bheap_size(&heap) );
////
////		bheap_pop( &heap );
////	}
////
////	bheap_destroy( &heap );
////	return 0;
////}
////
////
////int int_compare( const void* left, const void* right )
////{
////	const int* p_left = left;
////	const int* p_right = right;
////
////	#ifdef MAX_HEAP	
////	return (*p_left) - (*p_right); // max-heap
////	#else
////	return (*p_right) - (*p_left); // min-heap
////	#endif
////}
////
////boolean heap_elem_destroy( void *data )
////{
////	return TRUE;
////}
//
//
//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include "../multiwayMergesort.h"
//#include <assert.h>
////HEAP TEST _ TODO REMOVE
//#include "bheap.h"
//
///* type of file items */
//typedef unsigned long ItemType;
//////HEAP TEST _ TODO REMOVE
//#define MIN_HEAP	
//// main function
//typedef struct {
//    int k;
//    int offset;
//} heapElem2;
//
//int heapCompt(const void* a, const void* b) {
//    const heapElem2* left = a;
//    const heapElem2* right = b;
//    return (*right).offset - (*left).offset; // min-heap
//}
//
//boolean heap_elem_destroy(void *data) {
//    return TRUE;
//}
//
//
//int main(int argc, char** argv) {
//    //HEAP TEST _ TODO REMOVE
//    bheap_t heap;
//
//    bheap_create(&heap, sizeof (heapElem2), 1, heapCompt, heap_elem_destroy);
//    
//    heapElem2 t1;
//    t1.k =10;
//    t1.offset=16;    
//    bheap_push(&heap, &t1);
//    heapElem2 t2;
//    t2.k =2;
//    t2.offset=17;
//    bheap_push(&heap, &t2);
//
//
//    heapElem2* p_num = bheap_peek(&heap);
//    printf("c%d\n", p_num->k);
//    
//
//    bheap_pop(&heap);
//
//    p_num = bheap_peek(&heap);
//    printf( "c%d\n", p_num->k);
//
//    
//    return 0;
//}
//
