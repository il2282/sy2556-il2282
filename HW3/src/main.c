#include <stdio.h>
#include <stdlib.h>
#include "meanvar.h"
#include "power.h"
#include "utilities.h"

int engine(int numassets, int numeigval, double *mu, double *eigvec, double *eigval, double lambda);

int main(int argc, char *argv[])
{
  int assetnum, obsnum, code = 0, itr, j;
  double val, lambda;
  double *p_assetobs = NULL, *p_mean = NULL, *p_var = NULL, *p_vector = NULL, *p_eigval = NULL, *p_eigvec = NULL;
  FILE *input = NULL;
  char buffer[100];

  if(argc != 4){ 
    printf(" usage: main asset_observation_filename parameter_filename lambda\n");
    code = 1; goto BACK;
  }

  /* read parameters */
  input = fopen(argv[2], "r");
  if (input == NULL){
    printf("cannot read %s\n",argv[2]);
    code=2; goto BACK;
  }

  fscanf(input, "%s", buffer);
  fscanf(input, "%s", buffer);
  assetnum = atoi(buffer);

  fscanf(input, "%s", buffer);
  fscanf(input, "%s", buffer);
  obsnum = atoi(buffer);

  fclose(input);

  /* read asset historical price and calculate mean and variance.*/
  code = read_asset_obs_get_meanvar(argv[1], &p_assetobs, &p_mean, &p_var, assetnum, obsnum);
  if(code) goto BACK;

  p_vector = (double *)calloc(assetnum, sizeof(double));
  p_eigval = (double *)calloc(assetnum, sizeof(double));
  p_eigvec = (double *)calloc(assetnum*assetnum, sizeof(double));

  /* find large enough eigenvalues and their corresponding eigenvectors */
  for(itr = 0; itr < assetnum; itr++){

  	poweralg(assetnum, p_var, p_vector, &val);
  	p_eigval[itr] = val;
  	for (j = 0; j < assetnum; j++){
  		p_eigvec[itr*assetnum+j] = p_vector[j];
  	}

  	if (val/p_eigval[0] < 0.001) break;
  	matrix_subtraction(assetnum, p_var, p_vector, val);
  }

  /* run optimization problem */
  sscanf(argv[3], "%lf", &lambda);
  printf("in main: lambda = %f", lambda);
  code = engine(assetnum, itr, p_mean, p_eigvec, p_eigval, lambda);

BACK:
  printf("\nexiting main with code %d\n", code);
  return code;
}
