#include <stdio.h>

int main(void)
{
  FILE  *fpar;
  long int totalTerm,totalDocs;
  
  if((fpar=fopen("index-db/data.par","rb"))==NULL)
  {
    printf("Cannot open parameter file [data.par]...\n");
    return 0;
  }

  /* load parameter file */
  fread(&totalDocs,sizeof(long int),1,fpar);
  fread(&totalTerm,sizeof(long int),1,fpar);
  fclose(fpar);

  printf("Found %ld distict terms in %ld documents\n\n",totalTerm,totalDocs);  
  return 1;
}
