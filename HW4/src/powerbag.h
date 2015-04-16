#ifndef POWERBAG

#include "power.h"
#include "utilities.h"

#define POWERBAG


#define WAITING 100
#define WORKING 101
#define PREANYTHING 102
#define DONEWITHWORK 103
#define ENGINEPROBLEM 104


#define QUIT 200
#define WORK 201
#define STANDBY 202

typedef struct PowerBag{
  int rtnNum;
  int assetNum;
  double lambda;
  double *p_pertAssetRtn;
  double *p_mean;
  double *p_var;
  double *p_eigval;
  double *p_eigvec;
  double *p_vector;
  double *p_optimal;
  double tolerance;
  int ID;
  int status;
  int command;
  int jobNumber;  
  pthread_mutex_t *p_synchro;
  pthread_mutex_t *p_outputMutex;
}PowerBag;


int PWRallocatespace(PowerBag *p_bag);
void PWRfreespace(PowerBag *p_bag);
void poweralgWrapper(PowerBag *p_bag, int itr);
void matrixSubtractionWrapper(PowerBag *p_bag, int itr);
int engineWrapper(PowerBag *p_bag, int eigvalNum);

#endif