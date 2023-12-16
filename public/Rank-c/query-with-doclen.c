#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "index-tools.h"
#include "query-tools.h"
#include "util.h"
#include "define.h"

void query(FILE *, FILE *, Term *, char *, int, int);
int searchTerm(Term *, int, char *);
int getQueryTerm(char *, char[MAXQUERY][WORDLEN]);
int sort(Term *, int, char[MAXQUERY][WORDLEN]);

long int totalTerm, totalDocs;
StopList stoplist[TOTLIST];
FileInfo *fileinfo;

int main(int argc, char *argv[])
{
  Term *terms;
  FILE *fvoc, *finv, *fpar, *finf, *fnme;
  char buffer[BUFLEN], qry[BUFLEN];
  int i, len, HEAPSIZE;
  long int docno, doclen, offset;

  if (argc <= 1)
  {
    printf("input a query or more!!!\n");
    exit(1);
  }

  strcpy(qry, argv[1]);
  HEAPSIZE = argc > 2 ? atoi(argv[2]) : 15;

  if ((finv = fopen("index-db/data.inv", "rb")) == NULL)
  {
    printf("Cannot open inverted file [data.inv]...\n");
    return 0;
  }

  if ((fpar = fopen("index-db/data.par", "rb")) == NULL)
  {
    printf("Cannot open parameter file [data.par]...\n");
    return 0;
  }

  if ((fvoc = fopen("index-db/data.voc", "r")) == NULL)
  {
    printf("Cannot open vocabulary file [data.voc]...\n");
    return 0;
  }
  if ((finf = fopen("index-db/data.wdl", "r")) == NULL)
  {
    printf("Cannot open information file [data.wdl]...\n");
    return 0;
  }

  if ((fnme = fopen("index-db/data.nme", "r")) == NULL)
  {
    printf("Cannot open file [data.nme]...\n");
    return 0;
  }

  loadStopList(stoplist);

  /* load parameter file */
  fread(&totalDocs, sizeof(long int), 1, fpar);
  fread(&totalTerm, sizeof(long int), 1, fpar);
  fclose(fpar);

  printf("#Found %ld distict terms in %ld documents\n\n", totalTerm, totalDocs);
  /* allocate array files of type struct FileInfo and load file information
     in data.inf in the format of [docno] [offset to data.nme] into an array,
     so that binary search can be applied. The data in the array are sorted
     in ascending order */
  fileinfo = (FileInfo *)malloc(sizeof(FileInfo) * (totalDocs + 1));
  for (i = 0; fscanf(finf, "%ld %ld %ld", &docno, &doclen, &offset) == 3; i++)
  {
    fileinfo[i].docno = docno;
    fileinfo[i].doclen = doclen;
    fileinfo[i].offset = offset;
  }
  fclose(finf);

  /* allocate array terms of type struct Term and load terms in vocabulary
     file into an array, so that binary search can be applied. The data
     in the array are sorted in ascending order */
  terms = (Term *)malloc(sizeof(Term) * (totalTerm + 1));
  for (i = 0; fscanf(fvoc, "%s %d %ld", buffer, &len, &offset) == 3; i++)
  {
    terms[i].term = malloc(strlen(buffer) + 1);
    strcpy(terms[i].term, buffer);
    terms[i].len = len;
    terms[i].offset = offset;
  }
  fclose(fvoc);

  startTiming();
  query(finv, fnme, terms, qry, totalDocs, HEAPSIZE);
  stopTiming();

  printf("#Time required: %f mseconds\n", timingDuration());

  free(terms);
  free(fileinfo);
  fclose(finv);
  fclose(fnme);
  return 0;
}

void query(FILE *finv, FILE *fnme, Term *arrterm, char *query, int N, int HEAPSIZE)
{
  long int *ilbuf;
  long int freqwd, docno, offset, tempd;
  int i, t, pos, len, totalqterm;
  float fw;
  double *accumulator;
  double s_q_d, tempr;
  Heap *heap;
  char buffer[STRPATH];
  char queryterm[MAXQUERY][WORDLEN];

  if (*query != '\0')
  {
    totalqterm = getQueryTerm(query, queryterm); /*get only query terms that are not stopword */
    if (totalqterm == 0)
    {
      printf("All query terms are stopword\n");
    }
    else
    {
      heap = (Heap *)malloc(sizeof(Heap) * HEAPSIZE);

      /* sort query terms in ascending order based on fw of the term,
         rare term is more interesting and should be examined first */
      sort(arrterm, totalqterm, queryterm);

      /* allocate array of accumulator and set all to zero */
      accumulator = (double *)malloc(sizeof(double) * N);
      for (i = 0; i < N; i++)
      {
        accumulator[i] = 0;
      }

      /* for each query terms, calculate the similarity */
      for (i = 0; i < totalqterm; i++)
      {
        /* search query term in array of term */
        pos = searchTerm(arrterm, totalTerm, queryterm[i]);
        if (pos < 0)
        {
          printf("#Word ['%s'] is not indexed\n", queryterm[i]);
          
        }
        else
        {
          len = arrterm[pos].len;
          offset = arrterm[pos].offset;

          /* read inverted list for the term */
          fseek(finv, offset, 0);
          ilbuf = (long int *)malloc(sizeof(long int) * (len));
          fread(ilbuf, sizeof(long int), len, finv);
          fw = len / 2;
          printf("#Word ['%s'], fw (num of doc containing the word) = %f\n", queryterm[i], fw);

          /* for each pair, show the result */
          for (t = 0; t < fw; t++)
          {
            docno = ilbuf[t * 2];
            freqwd = ilbuf[t * 2 + 1];
            s_q_d = log(N / fw + 1) * log(freqwd + 1);
            accumulator[docno] += s_q_d;
          }
          free(ilbuf);
        }
      }

      /* Normalize accumulator by document length */
      for (i = 0; i < N; i++)
      {
        accumulator[i] += accumulator[i] / fileinfo[docno].doclen; /*using L(D)*/
      }

      /* build heap of size HEAPSIZE */
      for (i = 0; i < N; i++)
      {
        if (i < HEAPSIZE)
        {
          buildHeap(heap, i, i, accumulator[i]);
        }
        else
        {
          /* compare new value with the root of the heap, if the new value is
       larger then the root, insert the new value into the heap */
          if (accumulator[i] > heap[0].ranked)
          {
            heap[0].ranked = heap[HEAPSIZE - 1].ranked;
            heap[0].docno = heap[HEAPSIZE - 1].docno;
            heap[HEAPSIZE - 1].ranked = accumulator[i];
            heap[HEAPSIZE - 1].docno = i;
          }

          /* adjust heap */
          for (t = (HEAPSIZE / 2) - 1; t >= 0; t--)
          {
            adjustHeap(heap, t, HEAPSIZE);
          }
        }
      }

      /* sort heap: bubble sort */
      for (i = HEAPSIZE - 1; i > 0; i--)
      {
        for (t = 0; t < i; t++)
        {
          if (heap[t].ranked < heap[t + 1].ranked)
          {
            tempr = heap[t].ranked;
            tempd = heap[t].docno;
            heap[t].ranked = heap[t + 1].ranked;
            heap[t].docno = heap[t + 1].docno;
            heap[t + 1].ranked = tempr;
            heap[t + 1].docno = tempd;
          }
        }
      }

      printf("\n#Top %dth documents are:\n", HEAPSIZE);

      /* print heap */
      for (i = 0; i < HEAPSIZE; i++)
      {
        offset = fileinfo[heap[i].docno].offset;
        fseek(fnme, offset, 0);
        fgets(buffer, STRPATH, fnme);
        buffer[strlen(buffer) - 1] = '\0'; /*eliminate character '\n' sucked by fgets */
        // printf("Document [%s] or docno %ld ranked = %f\n",buffer,heap[i].docno,heap[i].ranked);
        printf("%s\t\t%f\n", buffer, heap[i].ranked);
      }
      
      free(accumulator);
    }
  }
}

/* getQueryTerm is the function to parse the query into
   several terms */
int getQueryTerm(char *query, char queryterm[MAXQUERY][WORDLEN])
{
  char word[WORDLEN];
  char *pw = word;
  char *pq = query;
  int totalqueryterm = 0;

  query[strlen(query)] = '\0'; /* replace character '\n' with '\0'I*/
  while (*pq != '\0')
  {
    while (isspace(*pq))
    {
      pq++;
    }

    while (*pq != '\0')
    {
      if (!isalnum(*pq))
      {
        pq++;
        break;
      }
      *pw = *pq;
      pq++;
      pw++;
    }
    *pw = '\0';
    if (strlen(word) != 0)
    {
      /* check whether the word is a stopword, if
   it is not, then add to queryterm array */
      if (!isstopword(word, TOTLIST, stoplist))
      {
        strcpy(queryterm[totalqueryterm++], word);
      }
    }
    word[0] = '\0';
    pw = word;
  }
  return totalqueryterm;
}

/* searchTerm is the function to search the uploaded terms from vocabulary
   file using binary search method */
int searchTerm(Term *arrterm, int num, char *query)
{
  int high = num - 1, low = 0, mid;
  int val;

  while (low <= high)
  {
    mid = (low + high) / 2;
    val = strcmp(arrterm[mid].term, query);
    if (val < 0)
    {
      low = mid + 1;
    }
    else if (val > 0)
    {
      high = mid - 1;
    }
    else
    {
      return mid;
    }
  }
	//rf
  return ((val == 0) ? mid : -1);
}

/* sort is a function to sort query terms by their frequency value in ascending
   order, rare term should examine first in order to heuristically limited the
   accumulator */
int sort(Term *arrterm, int totalqterm, char queryterm[MAXQUERY][WORDLEN])
{
  int i, t, pos;
  char temp[WORDLEN];
  float fw1, fw2;

  for (i = totalqterm - 1; i > 0; i--)
  {
    for (t = 0; t < i; t++)
    {
      pos = searchTerm(arrterm, totalTerm, queryterm[t]);
      fw1 = arrterm[pos].len / 2;
      pos = searchTerm(arrterm, totalTerm, queryterm[t + 1]);
      fw2 = arrterm[pos].len / 2;
      if (fw1 > fw2) /*sort in ascending order*/
      {
        strcpy(temp, queryterm[t]);
        strcpy(queryterm[t], queryterm[t + 1]);
        strcpy(queryterm[t + 1], temp);
      }
    }
  }
  return 1;
}
