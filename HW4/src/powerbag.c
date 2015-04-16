#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "utilities.h"
#include "powerbag.h"

int engine(int numassets, int numeigval, double *mu, double *eigvec, double *eigval, double lambda, double *optimalArr, pthread_mutex_t* output);


int PWRallocatespace(PowerBag *p_bag)
{
  int retcode = 0;
  double *v = NULL;
  int assetNum, rtnNum;

  rtnNum = p_bag->rtnNum;
  assetNum = p_bag->assetNum;

  v = (double *)calloc(assetNum*rtnNum + 4*assetNum + 2*assetNum*assetNum, sizeof(double));
  if(!v){
    retcode = NOMEMORY; goto BACK;
  }
  printf("allocated vector at %p\n", (void *) v);
  p_bag->p_pertAssetRtn = v;
  p_bag->p_mean = v + assetNum*rtnNum;
  p_bag->p_var = v + assetNum*rtnNum + assetNum;
  p_bag->p_eigval = v + assetNum*rtnNum + assetNum + assetNum*assetNum;
  p_bag->p_eigvec = v + assetNum*rtnNum + 2*assetNum + assetNum*assetNum;
  p_bag->p_vector = v + assetNum*rtnNum + 2*assetNum + 2*assetNum*assetNum;
  p_bag->p_optimal = v + assetNum*rtnNum + 3*assetNum + 2*assetNum*assetNum;

 BACK:
  return retcode;
}

int PWRfreespace(PowerBag *p_bag)
{
	int retcode = 0;

	return retcode;
}
void poweralgWrapper(PowerBag *p_bag, int itr)
{
  poweralg(p_bag->assetNum, p_bag->p_var, p_bag->p_vector, (p_bag->p_eigval)+itr);
}

void matrixSubtractionWrapper(PowerBag *p_bag, int itr)
{
  matrix_subtraction(p_bag->assetNum, p_bag->p_var, p_bag->p_vector, (p_bag->p_eigval)[itr]);
}

int engineWrapper(PowerBag *p_bag, int eigvalNum)
{
  return engine(p_bag->assetNum, eigvalNum, p_bag->p_mean, p_bag->p_eigvec, p_bag->p_eigval, p_bag->lambda, p_bag->p_optimal, p_bag->p_outputMutex);
}