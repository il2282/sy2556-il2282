#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utilities.h"

int readworkfile(char* workfile, char** wordlist, int* N);
int commandnerd(int N, int numnerds, char* wordlist, char* constfilename);
int sendword(char *destination, char* constfilename, char *word, int ID);

int main(int argc, char **argv)
{
  int runcode = 0, N, numnerds;
  char *wordlist = NULL;

  if(argc != 4){
    printf("master: usage is master workfile constfile numnerds outputfile\n"); runcode = 1; goto BACK;
  }

  numnerds = atoi(argv[3]);

  /* read workfile */
  runcode = readworkfile(argv[1], &wordlist, &N);
  if (runcode) goto BACK;

  /* command nerd */
  runcode = commandnerd(N, numnerds, wordlist, argv[2]);
  if (runcode) goto BACK;

 BACK:
  return runcode;
}


int readworkfile(char* workfile, char** wordlist, int* N)
{
  FILE *in;
  char buffer[100];
  int readcode = 0;

  in = fopen(workfile, "r");
  if(!in){
    printf("master: cannot open workfile %s\n", workfile); readcode = 3; goto BACK;
  }
  fscanf(in,"%s", buffer);
  *N = atoi(buffer);
  printf("master: N = %d\n", *N);
  fscanf(in,"%s", buffer); /* 'simulations' to be ignored */

  fscanf(in,"%s", buffer);
  *wordlist=strdup(buffer);

  fscanf(in,"%s", buffer);
  fclose(in);
  if(strcmp(buffer, "END")!=0){
    printf("master: wrong workfile format\n"); readcode = 4; goto BACK; /*TODO: Warning?*/
  }
  BACK:
   return readcode;
}


int commandnerd(int N, int numnerds, char* wordlist, char* constfilename)
{
  int commandcode = 0, initialN, numworking, workindex;
  int feasibleN = 0;
  double optval = 0, sum = 0, squredsum = 0, avg = 0, sd = 0;
  char **masterfilename = NULL;
  char **nerdfilename = NULL;
  char *busy = NULL;
  char buffer[100];
  FILE *in;
  int j;

  masterfilename = (char **)calloc(numnerds, sizeof(char *));
  nerdfilename = (char **)calloc(numnerds, sizeof(char *));
  busy = (char *)calloc(numnerds, sizeof(char));
  if(!masterfilename || !nerdfilename || !busy){
    printf("no memory\n"); commandcode = 2; goto BACK;
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
  
  initialN = (N < numnerds) ? N : numnerds;

  /** launch initial batch **/
  for(workindex = 0; workindex < initialN; workindex++){
    commandcode = sendword(masterfilename[workindex], constfilename, wordlist, workindex);
    if(commandcode) goto BACK;
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
            printf("master: cannot open nerdfile. Stop\n"); commandcode = 1; goto BACK;
          }
          fscanf(in,"%s",buffer);

          if(strcmp(buffer,"feasible") == 0){
			fscanf(in,"%s",buffer);
          	optval = atof(buffer);
          	++feasibleN;
          	sum += optval;
          	squredsum += optval*optval;
          }
          fclose(in);
          printf(" master:  nerd %d says: '%s'\n", j, buffer);
          erasefile(nerdfilename[j]);
          busy[j] = 0;
          --numworking;
          printf("master: now %d nerds working workindex %d\n", numworking,
           workindex);

          if(workindex < N){
            commandcode = sendword(masterfilename[j], constfilename, wordlist, j);
            if(commandcode) goto BACK;
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
    commandcode = sendword(masterfilename[j], "x", "x", j);
    if(commandcode) goto BACK;

  }
  
  printf("\nmaster: %d simulation(s) feasible, out of %d.\n", feasibleN, N);
  if(feasibleN != 0){
  	avg = sum/feasibleN;
  	sd = sqrt(squredsum/feasibleN-avg*avg);
  	printf("master: For the feasible samples,\n\toptimal cash flow average and standard deviation are %g and %g respectively.\n", avg, sd);
  }
  
  
  

  printf("master: I'm done\n");

  BACK:
    return commandcode;
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