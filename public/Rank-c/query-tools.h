/*
* query-tools.h
*
* Author: Taufik F. Abidin
*
*/


/* struct for loading data from file vocabulary (fvoc) */
typedef struct term
{
  char *term;
  int   len;
  int   offset;
}Term;


/* struct of heap */
typedef struct heap
{
  long int docno;
  double ranked;
}Heap;


/* struct for accumulator */
typedef struct accumulator
{
  unsigned long docno;
  float ranked;
}Accu;


/* struct for holding docno, doclen and offset. The offset
   is required to retrieve file name in file data.nme */
typedef struct fileinfo
{
  long int docno;
  long int doclen;
  long int offset;
}FileInfo;


/* functions implemented in query-tools.c */
int buildHeap(Heap *, int, unsigned long, double);
int adjustHeap(Heap *, int, int);
