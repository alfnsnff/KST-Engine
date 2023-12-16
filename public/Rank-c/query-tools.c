/*
* query-tools.h
*
* Author: Taufik F. Abidin
*
*/

#include "query-tools.h"
#include "define.h"

/* build min heap */
int buildHeap(Heap *heap, int n, unsigned long newdocno, double newranked)
{
  int i=n;

  heap[i].ranked=newranked;
  heap[i].docno=i;
  while((i>0) && (heap[(i-1)/2].ranked > newranked))
  {
    heap[i].ranked=heap[(i-1)/2].ranked;
    heap[i].docno=heap[(i-1)/2].docno;
    i=(i-1)/2;
  }
  heap[i].ranked=newranked;
  heap[i].docno=newdocno;
  return 1;
}


/* adjust heap */
int adjustHeap(Heap *heap, int pos, int HEAPSIZE)
{
  double num = heap[pos].ranked;
  unsigned long docno = heap[pos].docno;
  int i = 2*(pos+1)-1;

  while(i<=HEAPSIZE-1)
  {
	if((i<HEAPSIZE-1) && (heap[i].ranked > heap[i+1].ranked))
	{
	  i++;
    }
    if(num<=heap[i].ranked)
    {
	  break;
    }
    heap[(i-1)/2].ranked = heap[i].ranked;
    heap[(i-1)/2].docno = heap[i].docno;
    i= 2*(i+1)-1;
  }
  heap[(i-1)/2].ranked=num;
  heap[(i-1)/2].docno=docno;
	//rf
  return 1;
}
