#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "decomposer.h"

int drawuniform_generic(int a, int b){
	return rand()*(b-a)/((double) RAND_MAX)+a;
}
int drawuniform(){
	return rand()*4/((double) RAND_MAX)-2;
}

int getstdev(int numassets, int numperiods, double *matrix, double **p_mean, double **p_stdev){
	int retcode = 0;
	double *mean = NULL, *stdev = NULL;
	int i,j;

	mean = (double *)calloc(2*numassets, sizeof(double));
	if (!mean){
		printf("getstdev:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACK;
	}
	stdev = mean + numassets;

	for (j = 0; j < numassets; j++){
		mean[j] = 0.0;
		stdev[j] = 0.0;
		for (i = 0; i < numperiods; i++){
			mean[j] += matrix[j*numperiods+i];
			stdev[j] += matrix[j*numperiods+i]*matrix[j*numperiods+i];
		}
		mean[j] /= numperiods;
		stdev[j] /= numperiods;
		stdev[j] -= mean[j]*mean[j];
	}

	*p_mean = mean;
	*p_stdev = stdev;

BACK:
	return retcode;
}


int getstatistics(int numassets, int numperiods, double *matrix, double **p_mean, double **p_cov){
	int retcode = 0;
	double *mean = NULL, *cov = NULL;
	int i,j,k;

	mean = (double *)calloc(numassets*numassets + numassets, sizeof(double));
	if (!mean){
		printf("getstatistics:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACK;
	}
	cov = mean + 2*numassets;


	for (j = 0; j < numassets; j++){
		mean[j] = 0.0;
		for (i = 0; i < numperiods; i++){
			mean[j] += matrix[j*numperiods+i];
		}
		mean[j] /= numperiods;
	}

	for (k = 0; k < numassets; k++){
		for (j = k; j < numassets; j++){
			for (i = 0; i < numperiods; i++){
				cov[k*numassets+j] = matrix[k*numperiods+i]*matrix[j*numperiods+i];
			}
			cov[k*numassets+j] = cov[k*numassets+j]/numperiods - mean[k]*mean[j];
		}
	}
	for (k = 1; k < numassets; k++){
		for (j = j+1; j < numassets; j++){
			cov[j*numassets+k] = cov[k*numassets+j];
		}
	}

	*p_mean = mean;
	*p_cov = cov;

BACK:
	return retcode;

}

int perturbreturns(int numassets, int numperiods, double *assetreturns, double *stdev, double **p_perturbed){
	int retcode = 0;
	double *perturbed = NULL;
	int j, i;

	perturbed = (double *)calloc(numperiods*numassets, sizeof(double));
	if (!perturbed){
		printf("perturbreturns:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACK;
	}	
	
	for (j = 0; j < numassets; j++){
		for (i = 0; i < numperiods; i++){
			perturbed[j*numperiods+i] = assetreturns[j*numperiods+i] + drawuniform()*stdev[j];
		}
	}

	*p_perturbed = perturbed;

BACK:
	return retcode;
}