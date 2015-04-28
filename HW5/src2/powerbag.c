#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "powerbag.h"
#include "decomposer.h"


int engine(int numassets, int numeigval, double *mu, double *eigvec, double *eigval, double lambda, double *optimalArr, double *optimalval, pthread_mutex_t* output);


int PWRallocatespace(PowerBag *p_bag)
{
  int retcode = 0;
  double *v = NULL;
  int numassets, numperiods;

  numassets = p_bag->numassets;
  numperiods = p_bag->numperiods;

  v = (double *)calloc(numperiods*numassets + 2*numassets*numassets + 4*numassets, sizeof(double));
  if(!v){
  	printf("PWRallocatespace:: NO MEMORY\n");
    retcode = NOMEMORY; goto BACK;
  }

  p_bag->perturbed = v;
  p_bag->mean = v + numperiods*numassets;
  p_bag->cov = v + numperiods*numassets + numassets;
  p_bag->tmpvec = v + numperiods*numassets + numassets*numassets + numassets;
  p_bag->eigval = v + numperiods*numassets + numassets*numassets + 2*numassets;
  p_bag->eigvec = v + numperiods*numassets + numassets*numassets + 3*numassets;
  p_bag->portfolio = v + numperiods*numassets + 2*numassets*numassets + 3*numassets;


 BACK:
  return retcode;
}

void PWRfreespace(PowerBag *p_bag)
{
  if(p_bag->perturbed) free(p_bag->perturbed);
}

void getstatisticsWrapper(PowerBag *p_bag){
	getstatistics(p_bag->numassets, p_bag->numperiods, p_bag->perturbed, &p_bag->mean, &p_bag->cov);
}
void poweralgWrapper(PowerBag *p_bag, int itr)
{
  poweralg(p_bag->numassets, p_bag->cov, p_bag->tmpvec, (p_bag->eigval)+itr);
}

void matrixSubtractionWrapper(PowerBag *p_bag, int itr)
{
  matrix_subtraction(p_bag->numassets, p_bag->cov, p_bag->tmpvec, (p_bag->eigval)[itr]);
}

/*int engineWrapper(PowerBag *p_bag, int numeigval)
{
  return engine(p_bag->numassets, numeigval, p_bag->mean, p_bag->eigvec, p_bag->eigval, p_bag->risk_aversion, p_bag->portfolio, p_bag->output);
}*/