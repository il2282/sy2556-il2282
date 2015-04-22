
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readstructure(char *filename, int *pmax_names, double **ptheta, int numassets,
		  char **pparamsfilename, char **pinitialweightsfilename);
int engine(int numassets, int numfactors, 
	   double *ub, double *lb, double *mu, double *sigma2, 
	   double *V, double *F, int max_names, double *theta, 
	   double *initx, char *paramsfilename);

int readdata(char *filename, int *pnumassets, int *pnumfactors,
	     double **pub, double **plb, double **pmu, double **psigma2, double **pV, 
	     double **pF);

int readinitialweights(char *initialweightsfilename, double **pinit, 
				 int numassets);

int main(int argc, char **argv)
{
  int retcode = 0;
  int numassets, numfactors;
  double *ub = NULL, *lb = NULL, *mu = NULL, *sigma2 = NULL, *V = NULL, 
    *F = NULL, *theta = NULL, *initx = NULL;
  int max_names = 0;
  char *paramsfilename = NULL;
  char *initialweightsfilename = NULL;

  if( (argc < 2) || (argc > 3)){
    printf("usage: factorqp filename [structurefile]\n"); retcode = 1; goto BACK;
  }

  retcode = readdata(argv[1], &numassets, &numfactors,
	     &ub, &lb, &mu, &sigma2, &V, &F);
  if(retcode) goto BACK;

  printf("numassets: %d; numfactors: %d\n", numassets, numfactors);

  max_names = numassets;

  if(argc == 3){
    retcode = readstructure(argv[2], &max_names, &theta, numassets, &paramsfilename, &initialweightsfilename);
    if(retcode) goto BACK;
  }
  if(initialweightsfilename){
    retcode = readinitialweights(initialweightsfilename, &initx, 
				 numassets);
    if(retcode) goto BACK;
  }

  retcode = engine(numassets, numfactors, ub, lb, mu, sigma2, V, F, max_names,
		   theta, initx, paramsfilename);

 BACK:
  printf("\nexiting with retcode %d\n", retcode);
  return retcode;
}

int readdata(char *filename, int *pnumassets, int *pnumfactors,
	     double **pub, double **plb, double **pmu, double **psigma2, 
	     double **pV, double **pF)
{
  int retcode = 0;
  FILE *input;
  char buffer[100];
  int numassets = 0,  numfactors = 0, i, j, numcols;
  double *ub = NULL, *lb = NULL, *mu = NULL, *sigma2 = NULL, *V = NULL, *F = NULL;
  
  input = fopen(filename, "r");
  if(!input){
    printf("cannot read file %s\n", filename); retcode = 1; goto BACK;
  }
  fscanf(input,"%s", buffer);   printf("-> %s\n", buffer);
  fscanf(input,"%s", buffer);
  numassets = atoi(buffer);  printf("  %d assets\n", numassets);

  fscanf(input,"%s", buffer);   printf("-> %s\n", buffer);
  fscanf(input,"%s", buffer); 
  numfactors = atoi(buffer); printf("  %d factors\n", numfactors);


  numcols = 4*numassets + numfactors;  /** assets + binary variables + 
					   delta variables + factor variables **/

  mu = NULL;
  mu = (double *)calloc(5*numcols, sizeof(double));
  if(mu == NULL){
    retcode = 1; goto BACK;
  }
  ub = mu + numcols;
  lb = ub + numcols;
  sigma2 = lb + numcols; /* some waste here */
  V = (double *)calloc(numfactors*numassets + numfactors*numfactors, sizeof(double));
  if(V == NULL){
    retcode = 1; goto BACK;
  }
  
  F = V + numfactors*numassets;


  /* read returns */
  fscanf(input,"%s", buffer); 
  printf("-> %s\n", buffer);
  for(j = 0; j < numassets; j++){
	  fscanf(input,"%s", buffer); mu[j] = atof(buffer);
  }

  /* read ub */
  fscanf(input,"%s", buffer);
  printf("-> %s\n", buffer);
  for(j = 0; j < numassets; j++){
    fscanf(input,"%s", buffer); ub[j] = atof(buffer); lb[j] = -atof(buffer);
  } 

  /* read sigma2 */
  fscanf(input,"%s", buffer);
  printf("-> %s\n", buffer);
  for(j = 0; j < numassets; j++){
	  fscanf(input,"%s", buffer); sigma2[j] = atof(buffer);
  } 

  /* read V */
  fscanf(input,"%s", buffer);
  printf("-> %s\n", buffer);
  for(j = 0; j < numassets; j++)for(i = 0; i < numfactors; i++){
	  fscanf(input,"%s", buffer);
	  V[i*numassets + j] = atof(buffer);
  }

  /* read F */
  fscanf(input,"%s", buffer); 
  printf("-> %s\n", buffer);
  for(j = 0; j < numfactors; j++)for(i = 0; i < numfactors; i++){
	  fscanf(input,"%s", buffer);
	  F[i*numfactors + j] = atof(buffer);
  } 

  fscanf(input,"%s", buffer);
  if(0 != strcmp(buffer, "END")){
	  printf(" --> no END\n"); retcode = 1; goto BACK;
  }

  if(input) fclose(input);
 
  
  /**next, the upper bounds on the factor variables **/
  for(j = 0; j < numfactors; j++){
	  ub[j + numassets] = 100000.0; 
	  lb[j + numassets] = -100000.00;
  }
    
  *pnumassets = numassets;
  *pnumfactors = numfactors;
  *pub = ub; *plb = lb; *pmu = mu; *psigma2 = sigma2; *pV = V; *pF = F;
 BACK:
  return retcode;
}

int readstructure(char *filename, int *pmax_names, double **ptheta, int numassets,
		  char **pparamsfilename, char **pinitialweightsfilename)
{
  int retcode = 0;
  FILE *input;
  char buffer[100];
  int max_names = numassets, j, k;
  double *theta = NULL;
  
  input = fopen(filename, "r");
  if(!input){
    printf("cannot read file %s\n", filename); retcode = 1; goto BACK;
  }

  printf("reading structure file %s\n", filename); 
  for(;;){
    fscanf(input,"%s", buffer); 
    if(0 == strcmp(buffer, "max_names")){
      fscanf(input,"%s", buffer);
      max_names = atoi(buffer);
      max_names = (max_names < numassets) ? max_names : numassets;
      printf("max no. of names in portfolio: %d\n", max_names);
      *pmax_names = max_names;
    }
    else if(0 == strcmp(buffer, "theta")){
      if(( theta = (double *)calloc(numassets, sizeof(double))) == NULL){
	printf("cannot allocate theta\n"); retcode = 1; goto BACK;
      } 
      for(j = 0; j < numassets; j++){
	fscanf(input,"%s", buffer); 
	k = atoi(buffer);
	fscanf(input,"%s", buffer); 
	theta[k] = atof(buffer);
	printf("theta[%d] = %g\n", k, theta[k]);
      }
      *ptheta = theta;
      
    }
    else if(0 == strcmp(buffer, "gurobiparamsfile")){
      fscanf(input,"%s", buffer); 
      *pparamsfilename = strdup(buffer);
    }
    else if(0 == strcmp(buffer, "initialweights")){
      fscanf(input,"%s", buffer); 
      *pinitialweightsfilename = strdup(buffer);
    }
    else if(0 == strcmp(buffer, "END")){
      break;
    }
    else{
      printf("illegal buffer %s in structure file\n", buffer); retcode = 1;
      break;
    }
  }
  fclose(input);
 BACK:
  return retcode;
}


int readinitialweights(char *initialweightsfilename, double **pinitx, 
				 int numassets)
{
  int retcode = 0;
  FILE *input;
  char buffer[100];
  int j, count;
  double *ix = NULL;

  input = fopen(initialweightsfilename, "r");
  if(!input){
    printf("cannot open initial weights file %s\n", initialweightsfilename);
    retcode = 1; goto BACK;
  }
  printf("reading initial weights file %s\n", initialweightsfilename);
  if(!(ix = (double *)calloc(numassets, sizeof(double)))){
    printf("cannot allocate ix\n"); retcode = 1; goto BACK;
  }
  *pinitx = ix;
  
  count = 0;
  for(;;){
    fscanf(input,"%s", buffer); 
    if(0 == strcmp(buffer,"END")){
      printf("read in %d initial weights\n", count); break;
    }
    j = atoi(buffer + 1); /* why */
    fscanf(input,"%s", buffer);
    fscanf(input,"%s", buffer); ix[j] = atof(buffer);
    /*    printf("count %d %d %g\n", count, j, ix[j]);*/
    ++count;
  }



 BACK:
  return retcode;
}
