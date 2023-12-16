/*
* util.h
*
* Author: Taufik F. Abidin
*/

/* struct for loading stoplist */
typedef struct stoplist
{
  char *word;
}StopList;

/* function prototypes */
void getword(FILE *, char *);
void wordToLower(char *);

int binarySearch(StopList *, int, char *);
int loadStopList(StopList []);
int isstopword(char *, int, StopList *);
int startTiming(void);
int stopTiming(void);

float timingDuration(void);
