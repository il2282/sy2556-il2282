#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "inputreader.h"
#include "decomposer.h"
#include "powerbag.h"

void *workerWrapper(void *voided_bag);
void workerOptimization(PowerBag *p_bag);

int main(int argc, char **argv){
	int retcode = 0;
	int numassets, numperiods;
	char *constr_filename = NULL, *ruleone_filename = NULL, *ruletwo_filename = NULL;
	double initial_investment, risk_aversion, threshold, tolerance, czero;
	int investment_horizon;
	
	double *mean = NULL, *stdev = NULL, *price = NULL, *assetreturns = NULL, *perturbed = NULL;
	double *initx = NULL, *ub = NULL, *lb = NULL;
	int numr_one = 0, numr_two = 0;
	RuleOne *r_one = NULL; RuleTwo *r_two = NULL;

	int numsimulations, numworkers, activeworkers, jobcounter;
	PowerBag **powerbags = NULL, *p_bag = NULL;
	pthread_t *threads;
	pthread_mutex_t output;
	pthread_mutex_t *synchs;
	int j;
	

	if (argc != 6){
		printf("main:: usage: main datafile parameterfile structurefile numsimulations numworkers\n"); retcode = 100; goto BACK;
	}


	retcode = readparameter(argv[2], &numassets, &numperiods, &initial_investment, &investment_horizon, &risk_aversion, &threshold, &tolerance, &czero); 
	if (retcode) goto BACK;
	printf("main:: numassets: %d; numperiods: %d;\n       initial_investment: %g; investment_horizon: %d; risk_aversion: %g;\n       main:: threshold: %g; tolerance: %g\n", 
			numassets, numperiods, initial_investment, investment_horizon, risk_aversion, threshold, tolerance);
	
	retcode = readdata(argv[1], numassets, numperiods, &price, &assetreturns);
	if (retcode) goto BACK;

	retcode = readstructure(argv[3], &constr_filename, &ruleone_filename, &ruletwo_filename);
	if (retcode) goto BACK;
	printf("main:: constr_file: %s; ruleone_file: %s; ruletwo_file: %s\n", 
			constr_filename, ruleone_filename, ruletwo_filename);

	retcode = readconstr(constr_filename, numassets, &initx, &ub, &lb);
	if (retcode) goto BACK;
	
	if (ruleone_filename){
		retcode = readruleone(ruleone_filename, &numr_one, &r_one);
		if (retcode) goto BACK;
	}
	if (ruletwo_filename){
		retcode = readruletwo(ruletwo_filename, &numr_two, &r_two);
		if (retcode) goto BACK;
	}

	getstdev(numassets, numperiods, assetreturns, &mean, &stdev);


	numsimulations = atoi(argv[4]);
	numworkers = atoi(argv[5]);
	numworkers = (numworkers > numsimulations)? numsimulations : numworkers;

	printf("\nmain:: %d workers will work on %d simulations.\n", numworkers, numsimulations);

	/* begin of multi-threading */
	pthread_mutex_init(&output, NULL);

	synchs = (pthread_mutex_t *)calloc(numworkers, sizeof(pthread_mutex_t));
	if (!synchs){
		printf("main:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACK;
	}
	for (j = 0; j < numworkers; j++){
		pthread_mutex_init(&synchs[j], NULL);
	}

	threads = (pthread_t *)calloc(numworkers, sizeof(pthread_t));
	if (!threads){
		printf("main:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACK;
	}


	powerbags = (PowerBag **)calloc(numworkers, sizeof(PowerBag *));
	if (!powerbags){
		printf("main:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACK;
	}

	for (j = 0; j < numworkers; j++){
		powerbags[j] = (PowerBag *)calloc(1, sizeof(PowerBag));
		p_bag = powerbags[j];
    	
    	retcode = PWRallocatespace(p_bag);
    	if (retcode) goto BACK;

		p_bag->numperiods = numperiods;
		p_bag->numassets = numassets;
		p_bag->investment_horizon = investment_horizon;
		
		p_bag->risk_aversion = risk_aversion;
		p_bag->threshold = threshold;
		p_bag->tolerance = tolerance;

		p_bag->initx = initx;
		p_bag->ub = ub;
		p_bag->lb = lb;		

		p_bag->r_one = r_one;
		p_bag->r_two = r_two;

		p_bag->ID = j;
		p_bag->status = PREANYTHING;
		p_bag->command = STANDBY;

		p_bag->synch = &synchs[j];
		p_bag->output = &output;

		pthread_create(&threads[j], NULL, &workerWrapper, (void *) p_bag);

	}


	/* assing work */
	for (j = 0; j < numworkers; j++){
		p_bag = powerbags[j];

		pthread_mutex_lock(&synchs[j]);
		retcode = perturbreturns(numassets, numperiods, assetreturns, stdev, &perturbed);
		if (retcode) goto BACK;

		printf("assigned work %p\n", (void *) perturbed);

		p_bag->perturbed = perturbed;
		p_bag->status = WORKING;
		p_bag->command = WORK;
		p_bag->jobnumber = j;
		pthread_mutex_unlock(&synchs[j]);
		
	}

	jobcounter = activeworkers = numworkers;

	while (activeworkers > 0){

		for (j = 0; j < numworkers; j++){
			pthread_mutex_lock(&synchs[j]);
			p_bag = powerbags[j];
			if (p_bag->status == DONEWITHWORK){
				
				/* take the results TODO: */
				/* do something */


				if (jobcounter >= numsimulations){
					p_bag->status = QUITING;
					p_bag->command = QUIT;
					activeworkers--;
				} else {
					retcode = perturbreturns(numassets, numperiods, assetreturns, stdev, &perturbed);
					if (retcode) goto BACK;

					p_bag->perturbed = perturbed;
					p_bag->status = WORKING;
					p_bag->command = WORK;
					p_bag->jobnumber = jobcounter++;
				}

			} /*else if (p_bag->status == ENGINEPROBLEM){

			}*/
			pthread_mutex_unlock(&synchs[j]);
		}
	}


	

BACK:
	return retcode;
}

void *workerWrapper(void *voided_bag){
	PowerBag *p_bag = (PowerBag *) voided_bag;

	workerOptimization(p_bag);

	return (void *) &p_bag->ID;
}