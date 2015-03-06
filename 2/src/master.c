#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

int sendword(char *destination, char* constfilename, char *word, int ID);

int main(int argc, char **argv)
{
  int runcode = 0, N, j, numnerds, initialN, numworking, workindex;
  char buffer[100];
  FILE *out, *in;
  char **wordlist;
  char **masterfilename = NULL;
  char **nerdfilename = NULL;
  char *busy = NULL;

  if(argc != 4){
    printf("master: usage is master workfile constfile numnerds\n"); runcode = 1; goto BACK;
  }

  numnerds = atoi(argv[3]);

  masterfilename = (char **)calloc(numnerds, sizeof(char *));
  nerdfilename = (char **)calloc(numnerds, sizeof(char *));
  busy = (char *)calloc(numnerds, sizeof(char));
  if(!masterfilename || !nerdfilename || !busy){
    printf("no memory\n"); runcode = 2; goto BACK;
  }
  for(j = 0; j < numnerds; j++){
    masterfilename[j] = (char *)calloc(100, sizeof(char));
    nerdfilename[j] = (char *)calloc(100, sizeof(char));
    sprintf(masterfilename[j],"./tmp/masterfile%d.txt", j);
    sprintf(nerdfilename[j],"./tmp/nerdfile%d.txt", j);

    if(does_it_exist(masterfilename[j]))
      erasefile(masterfilename[j]);
    if(does_it_exist(nerdfilename[j]))
      erasefile(nerdfilename[j]);

  }


  /** read workfile -- crude **/
  in = fopen(argv[1], "r");
  if(!in){
    printf("master: cannot open workfile %s\n", argv[1]); runcode = 3; goto BACK;
  }
  fscanf(in,"%s", buffer);
  N = atoi(buffer);
  printf("master: N = %d\n", N);
  fscanf(in,"%s", buffer); /* 'simulations' to be ignored */

  wordlist = (char **)calloc(N, sizeof(char *));
  fscanf(in, "%s", buffer);
  for(j = 0; j < N; j++){
    /*if(EOF == fscanf(in,"%s", buffer)){
      printf("master: reached end of file after reading %d workds\n", j-1);
      printf("master: so resetting N to %d\n", j-1);
      N = j - 1;
      break;
    }*/
    wordlist[j] = strdup(buffer);
  }
  
  fscanf(in, "%s", buffer);
  fclose(in);

  if(strcmp(buffer, "END")!=0){
    printf("master: wrong workfile format\n"); runcode = 4; goto BACK; /*TODO: Warning?*/
  }

  initialN = (N < numnerds) ? N : numnerds;

  /** launch initial batch **/
  for(workindex = 0; workindex < initialN; workindex++){

    runcode = sendword(masterfilename[workindex], argv[2], wordlist[workindex], workindex);
    if(runcode) goto BACK;
    busy[workindex] = 1;
  }
  numworking = initialN;

  /** infinite cycle until all work done **/

  for(;;){
    printf("master: workindex %d numworking %d\n", workindex, numworking);
    if(numworking == 0)
      break;

    for(j = 0; j < numnerds; j++){
      if(busy[j]){
	/** check to see if done **/

      	if (does_it_exist(nerdfilename[j])){
      	  in = fopen(nerdfilename[j], "r");
      	  if(!in){
      	    printf("master: cannot open nerdfile. Stop\n"); runcode = 1; goto BACK;
      	  }
      	  fscanf(in,"%s",buffer);
      	  fclose(in);
      	  printf(" master:  nerd %d says: '%s'\n", j, buffer);
      	  erasefile(nerdfilename[j]);
      	  busy[j] = 0;
      	  --numworking;
      	  printf("master: now %d nerds working workindex %d\n", numworking,
      		 workindex);

      	  if(workindex < N){
      	    runcode = sendword(masterfilename[j], argv[2], wordlist[workindex], j);
      	    if(runcode) goto BACK;
      	    busy[j] = 1; /** nerd j is busy again **/
      	    ++numworking;

      	    ++workindex;  /** increment index of work done or being done**/
      	  }
      	}
      	else
      	  printf("master: nerd %d busy; workindex %d numworking %d\n", j,
      		 workindex, numworking);
      }

    }
  gotosleep(2);
  }

  /** now tell all nerds bye bye **/
  for(j = 0; j < numnerds; j++){
    runcode = sendword(masterfilename[j], "x", "x", j);
    if(runcode) goto BACK;

  }

  printf("master: I'm done\n");  
 BACK:
  return runcode;
}

int sendword(char *tokenfilename, char *constfilename, char *word, int ID)
{
  int sendcode = 0;
  FILE *out;

  printf("master: sending '%s' / '%s' to nerd %d using token file %s\n", 
    constfilename, word, ID, tokenfilename);
  out = fopen(tokenfilename, "w");
  if(!out){
    printf("master: cannot open token file. Stop\n"); sendcode = 1; goto BACK;
  }
  fprintf(out,"%s\n%s", constfilename, word);
  fclose(out);

 BACK:
  return sendcode;
}
