#ifndef POWERBAG

#include "power.h"
#include "rulebag.h"


#define POWERBAG

#define PREANYTHING 100
#define WORKING 101
#define DONEWITHWORK 102
#define QUITING 103
#define ENGINEPROBLEM 104


#define STANDBY 200
#define WORK 201
#define QUIT 202

#define NOMEMORY 10


typedef struct PowerBag{
	int numperiods, numassets;
	double *perturbed, *mean, *cov;
	double *initx, *ub, *lb;
	int investment_horizon;
	double risk_aversion, threshold, tolerance;

	double *eigval, *eigvec;
	double *tmpvec;
	double *portfolio;

	int numr_one, numr_two;
	RuleOne *r_one;
	RuleTwo *r_two;

	int ID;
	int status;
	int command;
	int jobnumber;
	pthread_mutex_t *synch;
	pthread_mutex_t *output;
}PowerBag;

int PWRallocatespace(PowerBag *p_bag);
void PWRfreespace(PowerBag *p_bag);

void getstatisticsWrapper(PowerBag *p_bag);
void poweralgWrapper(PowerBag *p_bag, int itr);
void matrixSubtractionWrapper(PowerBag *p_bag, int itr);
/*int engineWrapper(PowerBag *p_bag, int numeigval);*/


#endif
