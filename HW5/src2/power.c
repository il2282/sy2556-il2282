#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "power.h"



void poweriteration(int n, double *vector, double* newvector, double *matrix, double *p_error, double *p_val)
{
  double norm2 = 0, mult, norm = 0;
  int i, j;

  for(i = 0; i <n; i++){
    for (j = 0; j < n; j++){
      newvector[i] += vector[j]*matrix[i*n + j];
    }
  }

  norm2 = 0;
  for(j = 0; j < n; j++) norm2 += newvector[j]*newvector[j];

  norm = sqrt(norm2);
  *p_val = norm;
  mult = 1/norm;

  for(j = 0; j < n; j++) newvector[j] = newvector[j]*mult;

  compute_error(n, p_error, newvector, vector);

  /** will need to map newvector into vector if not terminated **/

  for(j = 0; j < n; j++) vector[j] = newvector[j];

}

void compute_error(int n, double *p_error, double *newvector, double *vector)
{
  int j;
  double error;

  error = 0;

  for(j = 0; j < n; j++){
    error += fabs(newvector[j] - vector[j]);
  }

  *p_error = error;

}

void poweralg(int n, double *matrix, double *p_vector, double *p_val, double tolerance){
  
  double error;
  double *newvector;
  int i;
  
  for(i = 0; i < n; i++){ 
  	p_vector[i] = rand()/((double) RAND_MAX);
  }

  newvector = (double *)calloc(n, sizeof(double));

  for(;;){
    poweriteration(n, p_vector, newvector, matrix, &error, p_val);
    if(error < tolerance) break;
  }

  if(newvector) free(newvector);
}


void matrix_subtraction(int n, double *matrix, double *p_eigvec, double eigval){
	int i, j;

	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){
			matrix[i*n+j] = matrix[i*n+j]-eigval*p_eigvec[i]*p_eigvec[j];
		}
	}
}