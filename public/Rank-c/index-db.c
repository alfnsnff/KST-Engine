/*
 * by tfa at informatika.unsyiah.ac.id/tfa
 *
 */

#include <sys/time.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "index-tools.h"
#include "util.h"
#include "define.h"

int main(void)
{
  FILE *fdat, *finv, *fvoc, *fpar, *fnme, *finf;
  DIR *dp;
  struct dirent *ep;
  char dirdata[STRPATH];
  long int docno, offset, doclen;
  char path[STRPATH];
  char word[WORDLEN];
  char class[WORDLEN];
  TreeRoot bst;
  StopList stoplist[TOTLIST];

  loadStopList(stoplist);
  initBST(&bst);

  if ((fnme = fopen("index-db/data.nme", "w")) == NULL)
  {
    printf("Cannot create data.nme file...\n");
    return 0;
  }
  if ((finf = fopen("index-db/data.wdl", "w")) == NULL)
  {
    printf("Cannot create file info without doclen [data.wdl]...\n");
    return 0;
  }
  if ((finv = fopen("index-db/data.inv", "wb")) == NULL)
  {
    printf("Cannot create inverted file [data.inv]...\n");
    return 0;
  }
  if ((fpar = fopen("index-db/data.par", "wb")) == NULL)
  {
    printf("Cannot create parameter file [data.par]...\n");
    return 0;
  }
  if ((fvoc = fopen("index-db/data.voc", "w")) == NULL)
  {
    printf("Cannot create vocabulary file [data.voc]...\n");
    return 0;
    //rf
  }

  printf("Data directory: ");
  scanf("%s", dirdata);
  startTiming();

  /* open directory where documents are stored */
  dp = opendir(dirdata);
  if (dp != NULL)
  {
    docno = 0;
    printf("\nWait, reading documents...\n");
    while ((ep = readdir(dp)))
    {
      if ((strcmp(ep->d_name, ".") != 0) && (strcmp(ep->d_name, "..") != 0))
      {
        path[0] = '\0';
        strcat(path, dirdata);
        strcat(path, ep->d_name);
        printf("path %s \n", path);
        if ((fdat = fopen(path, "r")) == NULL)
        {
          printf("Opening %s file [%s] failed...\n", path, ep->d_name);
          return 0;
        }
        else
        {
          if ((docno != 0) && ((docno % 1000) == 0))
          {
            printf("%ldth documents\n", docno);
          }
          doclen = 0;
          offset = ftell(fnme);

          /* read the doc and get all words */
          while (!feof(fdat))
          {
            getword(fdat, word); /* parse each words */
            if (isalpha(word[0]))
            {
              wordToLower(word); /* convert word to lower case */
              if (!isstopword(word, TOTLIST, stoplist))
              {
                insertBST(&bst, word, docno);
              }
            }
            doclen += strlen(word);
          }

          fprintf(fnme, "%ld\t%s\n", docno, ep->d_name);
          // fprintf(finf, "%ld\t%ld\t%ld\t%s\n", docno, doclen, offset, class); /* store docno, doclen & offset */
          fprintf(finf, "%ld %ld %ld\n", docno, doclen, offset); /* store docno, doclen & offset */

        }
        docno++;
        fclose(fdat);
      }
    }
    (void)closedir(dp);
    fclose(fnme);
    fclose(finf);
    printf("%ldth documents\n", docno);
  }
  else
  {
    printf("Cannot access data directory...\n");
    return 0;
  }

  /* record number of documents being indexed */
  fwrite(&docno, sizeof(long int), 1, fpar);

  /* do the tree traversal to read all data in the BST
     and store them in three separate files */
  printf("\nCreating inverted files...\n");
  treeTraversal(&bst, finv, fvoc, fpar);
  fclose(fvoc);
  fclose(fpar);
  fclose(finv);

  printf("Freeing memory...\n");
  freeTree(&bst);
  printf("\nGenerating index complete.\n");

  stopTiming();
  printf("Time required: %f seconds\n\n", timingDuration() * 1e-2);
  return 1;
}
