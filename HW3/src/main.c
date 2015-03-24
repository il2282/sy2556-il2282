#include <stdio.h>
#include <stdlib.h>
#include "meanvar.h"
#include "power.h"
#include "utilities.h"

int main(int argc, char *argv[])
{
  int assetnum, obsnum, code = 0, itr, j;
  double val;
  double *p_assetobs = NULL, *p_mean = NULL, *p_var = NULL, *p_vector = NULL, *p_eigval = NULL, *p_eigvec = NULL;
  FILE *input = NULL;
  char buffer[100];

  if(argc != 3){ 
    printf(" usage: main asset_observation_filename parameter_filename\n");
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


  

  for(itr = 0; itr < assetnum; itr++){

  	poweralg(assetnum, p_var, p_vector, &val);
  	p_eigval[itr] = val;
  	for (j = 0; j < assetnum; j++){
  		p_eigvec[j] = p_vector[j];
  	}
  	printf("The %dth eigenvalue = %f\n", itr, p_eigval[itr]);

  	if (val/p_eigval[0] < 0.001) break;

  	matrix_subtraction(assetnum, p_var, p_vector, val);
  }

BACK:
  return code;
}






























/*
	powerunit *punit = NULL;

	punit = new powerunit("two");
	punit->readnload(argv[1]);
	punit->iterate();

	delete punit;  */
 
