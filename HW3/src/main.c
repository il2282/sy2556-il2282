#include <stdio.h>
#include <stdlib.h>
#include "meanvar.h"

int main(int argc, char *argv[])
{
  int assetnum, obsnum, code = 0;
  double *p_assetobs = NULL, *p_mean = NULL, *p_var = NULL;
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

  printf("%f\n", p_var[1]);
  printf("%f\n", p_var[1000]);


	 
BACK:
  return code;
}




























/*
	powerunit *punit = NULL;

	punit = new powerunit("two");
	punit->readnload(argv[1]);
	punit->iterate();

	delete punit;  */
 
