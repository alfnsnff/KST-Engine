#define BUFLEN      256
#define WORDLEN     50
#define STRPATH	    512
#define TOTLIST     758     /* number of stopword in file stoplist */
#define MAXQUERY    25
/* used in classifier.c 

#define TOTTERM      14366    
#define TOTDOCS       2324
#define BITCHUNKSIZE    32
#define TOTTERMBIT   (TOTTERM/BITCHUNKSIZE)+1 */

/* used in query-all-terms.c */
#define TOTALTOPDOC TOTDOCS

/* DATADIR is used in file:
   - index-top.c 
   - query-all-terms.c */
#define DATADIR     "data/training-set-all/"  

