/* ============================================================================
 *  kWayMergeSort 
 * ============================================================================
 *  Irene Finocchi (finocchi@di.uniroma1.it)
 */

#define __MAC__

#ifndef __MAC__

#define _GNU_SOURCE
#define _LARGEFILE_SOURCE  
#define _LARGEFILE64_SOURCE  
#define _FILE_OFFSET_BITS 64

#endif

/* includes */
#include "multiwayMergesort.h"
#include <string.h>
#include <stdio.h>
//HM1
#include <sys/time.h>
#include <sys/resource.h>
#include "heap_ib/bheap.h"

#ifdef __MAC__

#define off64_t off_t
#define fopen64 fopen
#define fseeko64 fseeko
#define ftello64 ftello

#endif

/* constants */
#define DEBUG 1

/* local data types */
typedef struct {
    FILE* src; /* source file */
    FILE* des; /* destination file */
    size_t numSwaps; /* number of swaps of src and des files */

    off64_t N; /* number of items to be sorted */
    size_t itemSize; /* item size in bytes */

    size_t k; /* k ways of merging operations */
    size_t B; /* block size in bytes */

    size_t M; /* size in bytes of internal memory used to cache blocks
    							 M = k x B */

    char* itemCache; /* main memory cache containing k blocks of items */
    off64_t* offset; /* table of file offsets within runs to be merged */

    int (*compare)(const void*, const void*); /* item comparator */

    //hm1   
    bheap_t heap; //hm1 heap

    int verb; /* verbose flag */
    int errCode; /* operation error code */
} Data;


typedef unsigned long ItemType;

typedef struct {
    int k;
    off64_t offset;
    const void* val;
} heapElem;

/* table of error messages */
char* ErrTable[] = {
    "",
    "Can't open file",
    "Can't create temporary file",
    "Can't allocate memory",
    "File size is not a multiple of item size",
    "File read error",
    "File write error"
};

/* static function prototypes */
static int inMemSort(Data* d);
static int kWayMergeSort(Data *d);
static void swapFiles(Data* d);
static int runFormation(Data* d);
static int sortPasses(Data* d);
static int kMerge(Data* d, off64_t r, off64_t j);
static int initRuns(Data* d, off64_t r, off64_t j);
static size_t getMinRun(Data* d, off64_t r, off64_t j);
static int nextFrontItem(Data* d, off64_t r, off64_t j, size_t q);

//HM1
static int kMergeHeap(Data* d, off64_t r, off64_t j);
static int initRunsHeap(Data* d, off64_t r, off64_t j);
static size_t getMinRunHeap(Data* d, off64_t r, off64_t j);
static int nextFrontItemHeap(Data* d, off64_t r, off64_t j, size_t q);

static int copyBack(Data* d);
//HM1
static int heapComp(const void* a, const void* b);
static struct timeval timevaldiff(struct timeval *a, struct timeval *b);
static boolean heap_elem_destroy(void *data);

/* macros */
#define		  _EmptyRun(d,r,j,q)  ( (d)->offset[q] >= (d)->N || 			\
							 		(d)->offset[q] >= (j) + ((q)+1) * (r))
#define 	  _GetFrontItem(d,q)  ( (d)->itemCache + ( (q)*(d)->B +			\
							 		(d)->offset[q] % (d)->B )*(d)->itemSize)
//HM1 version - passo elemento strut elemHeap invece del indice del vettore
#define		  _EmptyRunHeap(d,r,j,hp)  ( (hp).offset >= (d)->N || 			\
							 		(hp).offset >= (j) + ((hp).k +1) * (r))
#define 	  _GetFrontHeapItem(d,hp)  ( (d)->itemCache + ( ((hp).k)*(d)->B +			\
							 		(hp).offset % (d)->B )*(d)->itemSize)

/* ----------------------------------------------------------------------------
 *  sort
 * ----------------------------------------------------------------------------
 * Sort a file of fixed-size items, using either stadard qsort (inMemSort)
 * or kWayMergeSort.
 * The function has very different memory requirements depending on the 
 * sorting algorithm being used.
 *
 * pathName: pathname of the file to be sorted
 * itemSize: size in bytes of file items
 * compare: item comparator
 * blockSize: block size B in items
 * k: number of blocks of items that can be cached in main memory
 * verbose: if nonzero, output sorting progress to stderr
 * return 0 upon successful completion, error code otherwise.
 */
int sort(const char* pathName, size_t itemSize,
        int (*compare)(const void*, const void*),
        size_t blockSize, size_t k, size_t externalFlag, int verbose) {

    Data d = {0};
    off64_t theFileSize;

    /* reset error code */
    d.errCode = 0;

    /* attempt to open input file for update (reading and writing) */
    d.src = fopen64(pathName, "r+");
    if (d.src == NULL) {
        d.errCode = CANT_OPEN_FILE;
        goto cleanup;
    }

    /* get input file size */
    fseeko64(d.src, 0, SEEK_END);
    theFileSize = ftello64(d.src);
    fseeko64(d.src, 0, SEEK_SET);

    /* check if file size is a multiple of inItemSize */
    if (theFileSize % itemSize != 0) {
        d.errCode = WRONG_FILE_SIZE;
        goto cleanup;
    }

    /* setup number of file items (N) */
    d.N = theFileSize / itemSize;

    /* setup item size */
    d.itemSize = itemSize;

    /* setup item comparator */
    d.compare = compare;

    /* setup verbose flag */
    d.verb = verbose;

    if (externalFlag) {
        /* init number of swaps */
        d.numSwaps = 0;

        /* setup block size (B) */
        d.B = blockSize;

        /* setup number of ways as number of blocks cached in internal memory */
        d.k = k;

        /* setup size in items of internal memory used to cache blocks */
        d.M = blockSize * k;

        /* create temporary file */
        d.des = tmpfile();
        if (d.des == NULL) {
            d.errCode = CANT_CREATE_TEMP_FILE;
            goto cleanup;
        }

        /* allocate block cache (of blockSize * k items) */
        d.itemCache = malloc(d.itemSize * d.M);
        if (d.itemCache == NULL) {
            d.errCode = CANT_ALLOCATE_MEMORY;
            goto cleanup;
        }

        /* allocate file offset table */
        d.offset = malloc(sizeof (unsigned long long)*d.k);
        if (d.offset == NULL) {
            d.errCode = CANT_ALLOCATE_MEMORY;
            goto cleanup;
        }

        /* dump relevant info to stderr in verbose mode */
        if (d.verb) {
            fprintf(stderr, "-- File indexing model: %lu bits\n", (unsigned long) sizeof (off64_t)*8);
            fprintf(stderr, "-- Item size:        %lu bytes\n", (unsigned long) d.itemSize);
            fprintf(stderr, "-- Block size (B):   %lu items (%.1f KB)\n",
                    (unsigned long) d.B, (d.B * d.itemSize) / 1024.0);
            fprintf(stderr, "-- Input size (N):   %llu items (%.1f MB)\n",
                    (unsigned long long) d.N, theFileSize / (1024.0 * 1024.0));
            fprintf(stderr, "-- Memory usage\n");
            fprintf(stderr, "--   Item cache (M): %lu items (%.1f MB)\n",
                    (unsigned long) d.M,
                    (d.M * d.itemSize) / (1024.0 * 1024.0));
            fprintf(stderr, "--   Offset table:   %lu indexes (%lu bytes)\n",
                    (unsigned long) d.k,
                    (unsigned long) ((sizeof (unsigned long long)*d.k)));
        }
    } else if (d.verb) {
        fprintf(stderr, "-- File indexing model: %lu bits\n", (unsigned long) sizeof (off64_t)*8);
        fprintf(stderr, "-- Item size:        %lu bytes\n", (unsigned long) d.itemSize);
        fprintf(stderr, "-- Input size (N):   %llu items (%.1f MB)\n",
                (unsigned long long) d.N, theFileSize / (1024.0 * 1024.0));
        fprintf(stderr, "-- Memory usage (M): %lu items (%.1f MB)\n",
                (unsigned long) d.N,
                (d.N * d.itemSize) / (1024.0 * 1024.0));
    }

    //call sort function
    if (externalFlag) return kWayMergeSort(&d);
    else return inMemSort(&d);

cleanup:

    if (d.offset != NULL) free(d.offset);
    if (d.itemCache != NULL) free(d.itemCache);
    if (d.src != NULL) fclose(d.src);
    if (d.des != NULL) fclose(d.des);

    return d.errCode;
}

/* ----------------------------------------------------------------------------
 *  kWayMergeSort
 * ----------------------------------------------------------------------------
 * Sort a file of fixed-size items.
 * The function requires B*itemSize + sizeof(off64_t)*k + O(1) bytes of RAM.
 * return 0 upon successful completion, error code otherwise.
 */

int kWayMergeSort(Data *d) {


    //HM1
    struct rusage runUsage;
    struct timeval startRun, stopRun;

    //START
    getrusage(RUSAGE_SELF, &runUsage);
    startRun = runUsage.ru_stime;

    /* make first pass by sorting consecutive runs of M bytes on the file */
    if (!runFormation(d)) goto cleanup;

    //STOP
    getrusage(RUSAGE_SELF, &runUsage);
    stopRun = runUsage.ru_utime;

    if (d->verb) {
        fprintf(stdout, "Run Formation started at: %ld.%lds\n", startRun.tv_sec, startRun.tv_usec);
        fprintf(stdout, "Run Formation ended at: %ld.%lds\n", stopRun.tv_sec, stopRun.tv_usec);

        struct timeval totRun = timevaldiff(&stopRun, &startRun);

        fprintf(stdout, "Run Formation time: %ld.%lds\n", totRun.tv_sec, totRun.tv_usec);
    }

    /* do remaining passes by repeatedly merging consecutive runs */
    if (!sortPasses(d)) goto cleanup;

    /* copy temp file back onto original file, if needed */
    if (!copyBack(d)) goto cleanup;

cleanup:

    if (d->offset != NULL) free(d->offset);
    if (d->itemCache != NULL) free(d->itemCache);
    if (d->src != NULL) fclose(d->src);
    if (d->des != NULL) fclose(d->des);

    return d->errCode;
}

/* ----------------------------------------------------------------------------
 *  runFormation
 * ----------------------------------------------------------------------------
 * Sort in internal memory consecutive runs of M items from source file and
 * append them to destination file
 */
static int runFormation(Data* d) {

    size_t theReadItems;

    if (d->verb) fprintf(stderr, "\n-- Run formation pass ");

    do {

        /* read up to M consecutive items from source file */
        theReadItems = fread(d->itemCache, d->itemSize, d->M, d->src);

        /* break if end of file reached */
        if (theReadItems == 0) break;

        /* sort them in internal memory */
        qsort(d->itemCache, theReadItems, d->itemSize, d->compare);

        /* append them to destination file */
        if (fwrite(d->itemCache, d->itemSize, theReadItems, d->des) != theReadItems)
            return (d->errCode = FILE_WRITE_ERROR, 0);

        if (d->verb) fprintf(stderr, ".");

    } while (theReadItems == d->M);

    if (d->verb) fprintf(stderr, " [done]\n");

    return 1;
}

/* ----------------------------------------------------------------------------
 *  sortPasses
 * ----------------------------------------------------------------------------
 * Make passes over the file starting from initial runs of M items, with runs
 * growing by a factor of k between consecutive passes. At each pass, repeatedly
 * merge groups of k consecutive sorted runs.
 */
static int sortPasses(Data* d) {

    off64_t runLen;

    /* make passes over the file */
    for (runLen = d->M; runLen < d->N; runLen *= d->k) {

        unsigned long long start;

        if (d->verb)
            fprintf(stderr, "-- Merging runs of %llu items into runs of %llu items\n",
                (unsigned long long) runLen, (unsigned long long) (runLen * d->k));

        /* exchange the role of source and destination files and rewind them */
        swapFiles(d);

        /* repeatedly merge consecutive groups of k sorted runs */
        for (start = 0; start < d->N; start = start + runLen * d->k) {
            
            /* merge k consecutive sorted runs of runLen items starting at item j */
            boolean useHeap = TRUE;
            if (!useHeap) {
                if (!kMerge(d, runLen, start)) return 0;
            } else {
                if (!kMergeHeap(d, runLen, start)) return 0;
            }

        }

        if (d->verb) fprintf(stderr, "   [done]\n");
    }

    return 1;
}

/* ----------------------------------------------------------------------------
 *  kMerge
 * ----------------------------------------------------------------------------
 * Merge k consecutive sorted runs of r items starting at item j.
 */
static int kMerge(Data* d, off64_t runLen, off64_t start) {

    unsigned long long i = 0;

    //HM1
    struct rusage mergeUsage;
    struct timeval startMerge, stopMerge;

    /* initialize runs */
    if (!initRuns(d, runLen, start)) return 0;

    //HM1-START MARGE
    getrusage(RUSAGE_SELF, &mergeUsage);
    startMerge = mergeUsage.ru_utime;


    //    int jj = 0;
    /* merge runs */
    for (;;) {

        /* get index of run containing the min item */
        size_t minRun = getMinRun(d, runLen, start);

        /* all runs are empty */
        if (minRun == (size_t) - 1) break;

        /* write the min item */
        if (fwrite(_GetFrontItem(d, minRun), d->itemSize, 1, d->des) != 1)
            return (d->errCode = FILE_WRITE_ERROR, 0);
        else if (d->verb) {
            i++;
            unsigned long long x = (runLen * d->k) > d->N ? d->N : (runLen * d->k);
            if (i % (x / 100) == 0) fprintf(stderr, ".");
        }

        /* get next front-item in the run */
        if (!nextFrontItem(d, runLen, start, minRun)) return 0;
    }

    //HM1
    getrusage(RUSAGE_SELF, &mergeUsage);
    stopMerge = mergeUsage.ru_utime;

    if (d->verb) {
        fprintf(stdout, "\nMerge started at: %ld.%lds\n", startMerge.tv_sec, startMerge.tv_usec);
        fprintf(stdout, "Merge ended at: %ld.%lds\n", stopMerge.tv_sec, stopMerge.tv_usec);

        struct timeval totMerge = timevaldiff(&stopMerge, &startMerge);

        fprintf(stdout, "Merge time: %ld.%lds\n", totMerge.tv_sec, totMerge.tv_usec);
    }

    if (d->verb) fprintf(stderr, "\ncompleted merge of %lu sequences\n",
            (unsigned long) d->k);

    return 1;
}

static int kMergeHeap(Data* d, off64_t runLen, off64_t start) {

    unsigned long long i = 0;

    //HM1
    struct rusage mergeUsage;
    struct timeval startMerge, stopMerge;

    //HM1 create heap
    boolean bheapCreate = bheap_create(&(d->heap), sizeof (heapElem), 1, heapComp, heap_elem_destroy);
    if (!bheapCreate) {
        d->errCode = CANT_ALLOCATE_HEAP;
        return 0;
    }

    /* initialize runs */
    if (!initRunsHeap(d, runLen, start)) return 0;

    //HM1-START MARGE
    getrusage(RUSAGE_SELF, &mergeUsage);
    startMerge = mergeUsage.ru_utime;
    //    int jj = 0;
    /* merge runs */
    for (;;) {

        /* get index of run containing the min item */
        size_t minRun = getMinRunHeap(d, runLen, start);


        /* all runs are empty */
        if (minRun == (size_t) - 1) break;


        heapElem* stp = bheap_peek(&(d->heap));

        /* write the min item */
        if (fwrite(_GetFrontHeapItem(d, *stp), d->itemSize, 1, d->des) != 1)
            return (d->errCode = FILE_WRITE_ERROR, 0);
        else if (d->verb) {
            i++;
            unsigned long long x = (runLen * d->k) > d->N ? d->N : (runLen * d->k);
            if (i % (x / 100) == 0) fprintf(stderr, ".");
        }

        /* get next front-item in the run */
        if (!nextFrontItemHeap(d, runLen, start, minRun)) return 0;
    }

    //HM1
    getrusage(RUSAGE_SELF, &mergeUsage);
    stopMerge = mergeUsage.ru_utime;

    if (d->verb) {
        fprintf(stdout, "\nMerge started at: %ld.%lds\n", startMerge.tv_sec, startMerge.tv_usec);
        fprintf(stdout, "Merge ended at: %ld.%lds\n", stopMerge.tv_sec, stopMerge.tv_usec);

        struct timeval totMerge = timevaldiff(&stopMerge, &startMerge);

        fprintf(stdout, "Merge time: %ld.%lds\n", totMerge.tv_sec, totMerge.tv_usec);
    }


    if (d->verb) fprintf(stderr, "\ncompleted merge of %lu sequences\n",
            (unsigned long) d->k);

    //Dealloco struttura heap _TODO
    //    heap_elem_destroy(&d->heap);

    return 1;
}

/* ----------------------------------------------------------------------------
 *  initRuns
 * ----------------------------------------------------------------------------
 * Initialize runs
 */
static int initRuns(Data* d, off64_t runLen, off64_t start) {

    size_t q, theReadItems;

    /* init runs */
    for (q = 0; q < d->k; ++q) {

        /* let p[q] be the index of the first item of the q-th run */
        d->offset[q] = start + q * runLen;
        
        /* if index d->offset[q] points past the end of the file, skip read operation
           as the run is empty */
        if (d->offset[q] >= d->N) continue;
        /* set the file position to the beginning of the q-th run */
        fseeko64(d->src, d->offset[q] * d->itemSize, SEEK_SET);

        /* read up to B consecutive items from source file */
        theReadItems = fread(_GetFrontItem(d, q), d->itemSize, d->B, d->src);

        /* check for file read error */
        if ((d->offset[q] + d->B > d->N && theReadItems != d->N % d->B) ||
                (d->offset[q] + d->B <= d->N && theReadItems < d->B))
            return (d->errCode = FILE_READ_ERROR, 0);
    }

    return 1;
}

static int initRunsHeap(Data* d, off64_t runLen, off64_t start) {

    size_t q, theReadItems;

    /* init runs */
    for (q = 0; q < d->k; ++q) {
        //init heap element
        heapElem ts;

        ts.offset = start + q * runLen;
        ts.k = q;

        /* if index d->offset[q] points past the end of the file, skip read operation
           as the run is empty */
        if (ts.offset >= d->N) continue;




        /* set the file position to the beg inning of the q-th run */
        fseeko64(d->src, ts.offset * d->itemSize, SEEK_SET);

        /* read up to B consecutive items from source file */
        theReadItems = fread(_GetFrontHeapItem(d, ts), d->itemSize, d->B, d->src);

        /* check for file read error */
        if ((ts.offset + d->B > d->N && theReadItems != d->N % d->B) ||
                (ts.offset + d->B <= d->N && theReadItems < d->B))
            return (d->errCode = FILE_READ_ERROR, 0);

        ts.val = _GetFrontHeapItem(d, ts); //puntatore al valore
        bheap_push(&(d->heap), &ts);
    }

    return 1;
}

/* ----------------------------------------------------------------------------
 *  nextFrontItem
 * ----------------------------------------------------------------------------
 * Let front item be the next item
 */
static int nextFrontItem(Data* d, off64_t runLen, off64_t start, size_t minRun) {

    size_t theReadItems;


    /* in minRun, advance front item pointer */
    d->offset[minRun]++;

    /* if the block is empty and the run is not empty, cache the next block */
    if (d->offset[minRun] % d->B == 0 && !_EmptyRun(d, runLen, start, minRun)) {

        /* set the file position to the next block within the q-th run */
        fseeko64(d->src, d->offset[minRun] * d->itemSize, SEEK_SET);

        /* read up to B consecutive items from source file */
        theReadItems = fread(_GetFrontItem(d, minRun), d->itemSize, d->B, d->src);

        /* check for file read error */
        if ((d->offset[minRun] + d->B > d->N && theReadItems != d->N % d->B) ||
                (d->offset[minRun] + d->B <= d->N && theReadItems < d->B))
            return (d->errCode = FILE_READ_ERROR, 0);
    }
    return 1;
}

static int nextFrontItemHeap(Data* d, off64_t runLen, off64_t start, size_t minRun) {

    size_t theReadItems;


    /* in minRun, advance front item pointer */
    heapElem *lshp;

    lshp = bheap_peek(&(d->heap));
    
    //new heap element to push
    heapElem ts;
    ts.k = minRun;
    ts.offset = lshp->offset + 1;


    bheap_pop(&(d->heap)); //delete last mim
    //    if (!_EmptyRunHeap(d, runLen, start, ts)) //aggiungo solo se valido CORRETTO?


    /* if the block is empty and the run is not empty, cache the next block */
    if (ts.offset % d->B == 0 && !_EmptyRunHeap(d, runLen, start, ts)) {

        /* set the file position to the next block within the q-th run */
        fseeko64(d->src, ts.offset * d->itemSize, SEEK_SET);

        /* read up to B consecutive items from source file */
        theReadItems = fread(_GetFrontHeapItem(d, ts), d->itemSize, d->B, d->src);



        /* check for file read error */
        if ((ts.offset + d->B > d->N && theReadItems != d->N % d->B) ||
                (ts.offset + d->B <= d->N && theReadItems < d->B))
            return (d->errCode = FILE_READ_ERROR, 0);
    }
    ts.val = _GetFrontHeapItem(d, ts); //puntatore al valore
    bheap_push(&(d->heap), &ts); //re-add
    return 1;
}

/* ----------------------------------------------------------------------------
 *  getMinRun
 * ----------------------------------------------------------------------------
 * Get index of run containing the min item, or -1 if all runs are empty
 */
static size_t getMinRun(Data* d, off64_t runLen, off64_t start) {

    size_t q, qmin = (size_t) - 1;

    /* scan runs */
    for (q = 0; q < d->k; ++q) {

        /* skip empty runs */
        if (_EmptyRun(d, runLen, start, q)) continue;

        /* check if current item is smaller than previous min */
        if (qmin == (size_t) - 1 || d->compare(_GetFrontItem(d, q), _GetFrontItem(d, qmin)) < 0)
            qmin = q;
    }

    return qmin;
}

static size_t getMinRunHeap(Data* d, off64_t runLen, off64_t start) {

    heapElem *st;

    //    if (bheap_size(&(d->heap)) < 1) return (size_t) - 1;

    //    int q = 0;
    //    do {
    //        if (q >= d->k) return (size_t) - 1;
    //        st = bheap_peek(&(d->heap));
    //        q++;
    //    } while (_EmptyRunHeap(d, runLen, start, *st));

    /* scan runs */
    //        int q;
    while (bheap_size(&(d->heap)) > 0) {
        st = bheap_peek(&(d->heap));
        /* skip empty runs */
        if (!_EmptyRunHeap(d, runLen, start, *st)) return st->k;
        bheap_pop(&(d->heap));
    }
    return (size_t) - 1;


    //    st = bheap_peek(&(d->heap));
    //    return st->k;
}

/* ----------------------------------------------------------------------------
 *  copyBack
 * ----------------------------------------------------------------------------
 * Copy temporary file back to original file, if needed.
 */
static int copyBack(Data* d) {
    size_t theReadItems;

    /* if number of swaps is an odd number, no copy is required */
    if (d->numSwaps % 2 != 0) return 0;

    /* exchange the role of source and destination files and rewind them */
    swapFiles(d);

    if (d->verb) fprintf(stderr, "-- Copying temporary file back to original file ");

    do {
        /* read up to M consecutive items from source file */
        theReadItems = fread(d->itemCache, d->itemSize, d->M, d->src);

        /* break if end of file reached */
        if (theReadItems == 0) break;

        /* append them to destination file */
        if (fwrite(d->itemCache, d->itemSize, theReadItems, d->des) != theReadItems)
            return (d->errCode = FILE_WRITE_ERROR, 0);

        if (d->verb) fprintf(stderr, ".");

    } while (theReadItems == d->M);

    if (d->verb) fprintf(stderr, " [done]\n");
    return 1;
}

/* ----------------------------------------------------------------------------
 *  inMemSort
 * ----------------------------------------------------------------------------
 * Run standard quicksort
 */
static int inMemSort(Data* d) {
    size_t theReadItems;

    fprintf(stderr, "\n--  Running standard qsort \n");

    /* allocate cache for all items */
    d->itemCache = malloc(d->itemSize * d->N); //599261000
    if (d->itemCache == NULL) {
        d->errCode = CANT_ALLOCATE_MEMORY;
        goto cleanup;
    }

    fprintf(stderr, "\n--  Memory allocation successful \n");

    /* read all items from source file */
    theReadItems = fread(d->itemCache, d->itemSize, d->N, d->src);

    /* break if end of file reached */
    if (theReadItems == 0) {
        d->errCode = FILE_READ_ERROR;
        goto cleanup;
    }

    /* sort them in internal memory */
    qsort(d->itemCache, theReadItems, d->itemSize, d->compare);

    fprintf(stderr, "    [done]\n");

cleanup:

    if (d->offset != NULL) free(d->offset);
    if (d->itemCache != NULL) free(d->itemCache);
    if (d->src != NULL) fclose(d->src);
    if (d->des != NULL) fclose(d->des);

    return d->errCode;
}

/* ----------------------------------------------------------------------------
 *  swapFiles
 * ----------------------------------------------------------------------------
 * Exchange the role of source and destination files and rewind them
 */
static void swapFiles(Data* d) {
    FILE* tmp = d->src;
    d->src = d->des;
    d->des = tmp;
    rewind(d->src);
    rewind(d->des);
    d->numSwaps++;
}

//hm1 
//Compute difference between two timeva value

static struct timeval timevaldiff(struct timeval *a, struct timeval *b) {
    struct timeval r;
    r.tv_sec = abs(a->tv_sec - b->tv_sec);
    r.tv_usec = abs(a->tv_usec - b->tv_usec);
    return r;
}

//struct heapElem _ Def ordinamento
static int heapComp(const void* a, const void* b) {
    const heapElem* p_left = (heapElem*) a;
    const heapElem* p_right = (heapElem*) b;
    
    return *(ItemType*) p_right->val - *(ItemType*) p_left->val;
}


//Deallocate heap structure - TODO: finish

static boolean heap_elem_destroy(void *data) {
    return TRUE;
}