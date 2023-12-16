#include <string.h>
#include <ctype.h>
#include <stdlib.h>
// Linux Sys
// #include <sys/times.h>

//Win Sys
#include <time.h>
#include "index-tools.h"
#include "util.h"
#include "define.h"


/* struct for timing */

// Linux Sys
// static struct tms _start; /* the starting time */
// static struct tms _stop;  /* the ending time */


//Win Sys
static clock_t _start; /* the starting time */
static clock_t _stop;  /* the ending time */


void getword(FILE *fd, char *word)
{
  int chr;
  char *pw=word;

  /* ignored if chr is a white space */
  while(isspace(chr=fgetc(fd)));
  if(isalpha(chr))
  {
    *pw++=chr;
  }
  while(chr!=EOF)
  {
	if(!isalnum(*pw=fgetc(fd)))
	  break;
	pw++;
  }
  *pw='\0';
}


int binarySearch(StopList *stoplist, int num, char *word)
{
  int high=num-1, low=0, mid;
  int val;

  while(low<=high)
  {
    mid=(low+high)/2;
    val=strcmp(stoplist[mid].word,word);
    if(val<0)
    {
      low=mid+1;
    }
    else if(val>0)
    {
      high=mid-1;
    }
    else
    {
      return (mid);
    }
  }
  return ((val==0) ? mid : -1);
}

/* check whether the word is a stopword */
int isstopword(char *word, int totlist, StopList *stoplist)
{
  /* check wheather the word found in the array
     of stoplist */
  if(binarySearch(stoplist,totlist,word)<0)
  {
    return 0;  /* not found */
  }
  return 1;    /* found */
}


/* convert word to lower case */
void wordToLower(char *word)
{
  char *pw=word;
  for(;*pw!='\0';pw++)
  {
    if(isupper(*pw))
    {
      *pw=tolower(*pw);
    }
  }
}

/* load stoplist to array */
int loadStopList(StopList stoplist[])
{
  int i;
  FILE * fstop;
  char buffer[WORDLEN];

  if((fstop=fopen("stoplist","r"))==NULL)
  {
    printf("Opening file stoplist failed...\n");
    return 0;
  }
  else
  {
    for(i=0;fscanf(fstop,"%s",buffer)==1;i++)
	{
	  stoplist[i].word=malloc(strlen(buffer)+1);
	  strcpy(stoplist[i].word,buffer);
    }
  }
  return 1;
}


int startTiming(void)
{
  // Linux Sys
  // times(&_start);

  //Win Sys
  _start = clock();
  return 1;
}

int stopTiming(void)
{
  // Linux Sys
  // times(&_stop);

  //Win Sys
  _stop = clock();
  return 1;
}

float timingDuration(void)
{
  /* multiply by 1e-2 to convert milisec to sec */

  // Linux Sys
  // return (_stop.tms_utime - _start.tms_utime);

  //Win Sys
  return ((float)(_stop - _start) / CLOCKS_PER_SEC);
}