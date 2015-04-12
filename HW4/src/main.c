#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "utilities.h"
#include "powerbag.h"
#include "seriesoperation.h"

void *workerWrapper(void *p_voidedbag);

int main(int argc, char *argv[])
{
  int code = 0, j, initialRuns, activeWorkers, scheduledJobs, assetIndex, assetNum, rtnNum;
  int quantity = 1, workersNum = 1;
  double *p_assetRtn = NULL, *p_mean = NULL, epsSd, *v = NULL, orgProp;
  double *p_fakeRtn = NULL;
  double lambda, tolerance;
  PowerBag **pp_bag = NULL, *p_bag = NULL;
  int theWorker;
  char gotone;
  FILE *input;
  char buffer[100];

  pthread_t *p_threadArray;
  pthread_mutex_t output;
  pthread_mutex_t *p_synchroArray;

  if(argc != 6){ 
    printf(" usage: rpower data_filename parameters_filename workersNum job_quantity lambda\n");
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
  assetNum = atoi(buffer);

  fscanf(input, "%s", buffer);
  fscanf(input, "%s", buffer);
  rtnNum = atoi(buffer)-1;

  fscanf(input, "%s", buffer);
  fscanf(input, "%s", buffer);
  epsSd = atof(buffer);
  if (epsSd <= 0){
    printf("Standard deviation must be positive.\n");
    code = 1;
    goto BACK;
  }

  fscanf(input, "%s", buffer);
  fscanf(input, "%s", buffer);
  orgProp = atof(buffer);

  fscanf(input, "%s", buffer);
  fscanf(input, "%s", buffer);
  tolerance = atof(buffer);

  fclose(input);

  workersNum = atoi(argv[3]);
  quantity = atoi(argv[4]);
  sscanf(argv[5], "%lf", &lambda);

  printf("%d workers will work on %d jobs.\n", workersNum, quantity);

  /* read data and obtain average return of assets */
  code = readAssetObsGetMean(argv[1], &p_assetRtn, &p_mean, assetNum, rtnNum);
  if(code) goto BACK;

  /* output mutex, common to everybody */
  pthread_mutex_init(&output, NULL); /** **/

  /* psynchronization mutex, unique to each worker */
  p_synchroArray = (pthread_mutex_t *)calloc(workersNum, sizeof(pthread_mutex_t));
  if(!p_synchroArray){
    printf("could not create mutex array\n"); code = NOMEMORY; goto BACK;
  }

  for(j = 0; j < workersNum; j++)
    pthread_mutex_init(&p_synchroArray[j], NULL);

  /* create bag */
  pp_bag = (PowerBag **)calloc(workersNum, sizeof(PowerBag *));
  if(!pp_bag){
    printf("could not create bag array\n"); code = NOMEMORY; goto BACK;
  }

  /* create thread */
  p_threadArray = (pthread_t *)calloc(workersNum, sizeof(pthread_t));
  if(!p_threadArray){
    printf("could not create thread array\n"); code = NOMEMORY; goto BACK;
  }

  /* create v*/
  v = (double *)calloc(assetNum, sizeof(double));
  if (!v){
    printf("could not create v\n");
    code = NOMEMORY;
    goto BACK;
  }
  drawNormalVector(v, assetNum, 1);

  /* create fake return array */
  p_fakeRtn = (double *)calloc(assetNum*rtnNum, sizeof(double));
  if (!p_fakeRtn){
    printf("could not create fake return array\n");
    code = NOMEMORY;
    goto BACK;
  }

  /* assign values to bags and launch threads.*/
  for(j = 0; j < workersNum; j++){

    
    pp_bag[j] = (PowerBag *)calloc(1, sizeof(PowerBag));
    p_bag = pp_bag[j];
    if(!p_bag){
      printf("cannot allocate bag for ID %d\n", j); code = NOMEMORY; goto BACK;
    }
    p_bag->rtnNum = rtnNum;
    p_bag->assetNum = assetNum;
    if((code = PWRallocatespace(p_bag))){
      printf("cannot allocate space to arrays in worker%d.\n", workersNum);
      code = NOMEMORY; goto BACK;
    }
    p_bag->ID = j;
    p_bag->lambda = lambda;
    p_bag->tolerance = tolerance;
    p_bag->p_mean = p_mean;
    p_bag->status = PREANYTHING;
    p_bag->command = STANDBY;
    p_bag->p_synchro = &p_synchroArray[j];
    p_bag->p_outputMutex = &output;

    printf("about to launch thread for worker %d\n", j);

    pthread_create(&p_threadArray[j], NULL, &workerWrapper, (void *) p_bag);
  }

  /* start jobs */
  initialRuns = (workersNum < quantity) ? workersNum : quantity;

  for(theWorker = 0; theWorker < initialRuns; theWorker++){
    p_bag = pp_bag[theWorker];

    pthread_mutex_lock(&output);
    printf("\nPerturbation starts\n");
    if((code = timeSeriesPerturb(p_assetRtn, p_bag, v, epsSd, orgProp)))
      goto BACK;
    printf("\nPerturbation ends\n");
    pthread_mutex_unlock(&output);

    pthread_mutex_lock(&output);
    printf("*****master:  worker %d will run experiment %d\n", theWorker, theWorker);
    pthread_mutex_unlock(&output);

    /** tell the worker to work **/
    pthread_mutex_lock(&p_synchroArray[theWorker]);
    p_bag->command = WORK;
    p_bag->status = WORKING;
    p_bag->jobNumber = theWorker;
    pthread_mutex_unlock(&p_synchroArray[theWorker]);

  }
  scheduledJobs = activeWorkers = initialRuns;

  while(activeWorkers > 0){
    /** check the workers' status **/
    gotone = 0;
    for(theWorker = 0; theWorker < workersNum; theWorker++){

      pthread_mutex_lock(&p_synchroArray[theWorker]);
      p_bag = pp_bag[theWorker];
      if(p_bag->status == DONEWITHWORK){

      	pthread_mutex_lock(&output);
      	printf("master:  worker %d is done with job %d\nOptimal variables:\n", p_bag->ID, p_bag->jobNumber);
        for (assetIndex = 0; assetIndex < assetNum; assetIndex++){
          if (p_bag->p_optimal[assetIndex] > 0.0001) printf("x%d = %.12e\n", assetIndex, p_bag->p_optimal[assetIndex]);
        }
      	pthread_mutex_unlock(&output);

      	if(scheduledJobs >= quantity){
      	  /** tell worker to quit **/
      	  pthread_mutex_lock(&output);
      	  printf("master: telling worker %d to quit\n", p_bag->ID);
      	  pthread_mutex_unlock(&output);
      	  p_bag->command = QUIT;
      	  p_bag->status = QUIT;
      	  --activeWorkers;
      	}
      	else {
      	  gotone = 1;
      	}
      }
      else if(p_bag->status == PREANYTHING){
      	pthread_mutex_lock(&output);
      	printf("master:  worker %d is available\n", theWorker);
      	pthread_mutex_unlock(&output);
      	gotone = 1;
      }
      pthread_mutex_unlock(&p_synchroArray[theWorker]);
      if(gotone) break;
      sleep(2);
      
    }
    /** at this point we have run through all workers **/

    if(gotone){
    /** if we are here, "theWorker" can work **/
      p_bag = pp_bag[theWorker];
      if((code = timeSeriesPerturb(p_assetRtn, p_bag, v, epsSd, orgProp)))
        goto BACK;

      pthread_mutex_lock(&output);
      printf("master:  worker %d will run experiment %d\n", theWorker, scheduledJobs);
      pthread_mutex_unlock(&output);

      /** tell the worker to work **/
      pthread_mutex_lock(&p_synchroArray[theWorker]);
      p_bag->command = WORK;
      p_bag->status = WORKING;
      p_bag->jobNumber = scheduledJobs;
      pthread_mutex_unlock(&p_synchroArray[theWorker]);

      ++scheduledJobs;
    }
  }



  /*  pthread_mutex_lock(&p_synchroArray[theWorker]);
  p_bag->command = QUIT;
  pthread_mutex_unlock(&p_synchroArray[theWorker]);*/


      
  /*PWRfreespace(&p_bag);*/

	 
BACK:
  /*if(matcopy) free(matcopy);
  if(scratch) free(scratch);*/
  return code;
}


void *workerWrapper(void *p_voidedBag)
{
  PowerBag *p_bag = (PowerBag *) p_voidedBag;

  workerOptimization(p_bag);

  return (void *) &p_bag->ID;
}





















