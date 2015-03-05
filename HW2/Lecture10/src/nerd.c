#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

int processfile(char *filename, double *pvalue, int nerdID);

int main(int argc, char **argv)
{
  int runcode = 0, ID;
  char buffer[100];
  char masterfilename[100];
  char nerdfilename[100];
  double finalcashflow;
  FILE *out, *in;

  if(argc != 2){
    printf("nerd: usage is nerd ID\n"); runcode = 1; goto BACK;
  }
  ID = atoi(argv[1]);

  sprintf(masterfilename,"masterfile%d.txt", ID);
  sprintf(nerdfilename,"nerdfile%d.txt", ID);

  printf("nerd%d: waiting for master\n", ID);
  for(;;){
    if (does_it_exist(masterfilename)){
      in = fopen(masterfilename, "r");
      if(!in){
	printf("nerd%d: cannot open masterfile. Stop\n", ID); runcode = 1; goto BACK;
      }
      fscanf(in,"%s",buffer);
      fclose(in);
      printf("nerd%d:   master says: %s\n", ID, buffer);
      erasefile(masterfilename);

      if(strcmp(buffer,"x") == 0)
	goto BACK;

      /** now do the work, baby **/
      runcode = processfile(buffer, &finalcashflow, ID);
      if(runcode) goto BACK;


      sprintf(buffer, "%g\n", finalcashflow);

      
      /** write it to file **/
      out = fopen(nerdfilename, "w");
      if(!out){
	printf("nerd%d: cannot open nerdfile. Stop\n", ID); runcode = 1; goto BACK;
      }
      fprintf(out,"%s",buffer);
      fclose(out);
      printf("nerd%d: waiting for master\n", ID);
    }
    gotosleep(1);
  }
  
 BACK:
  printf("nerd: bye bye\n");
  return runcode;
}
