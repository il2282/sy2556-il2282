#include <pthread.h>
#include <unistd.h>
#include "powerbag.h"
#include "utilities.h"

void workerOptimization(PowerBag *p_bag)
{
  int itr, ID, letsgo, waitcount;
  double tolerance;


  ID = p_bag->ID;
  tolerance = p_bag->tolerance;

  pthread_mutex_lock(p_bag->p_outputMutex);
  printf("ID %d starts\n", ID);
  pthread_mutex_unlock(p_bag->p_outputMutex);

  for(;;){

    pthread_mutex_lock(p_bag->p_outputMutex);
    printf(" ID %d in big loop\n", ID);
    pthread_mutex_unlock(p_bag->p_outputMutex);

    letsgo = 0;
    waitcount = 0;
    while(letsgo == 0){
      /** wait until first WORK signal **/
      sleep(1);

      pthread_mutex_lock(p_bag->p_synchro);
      if(p_bag->command == WORK){
        letsgo = 1;
      }
      else if(p_bag->command == QUIT)
        letsgo = 2;
      pthread_mutex_unlock(p_bag->p_synchro);

      if (letsgo == 2) 
        goto DONE;

      if(0 == waitcount%2){
        pthread_mutex_lock(p_bag->p_outputMutex);
        printf("ID %d: wait %d for signal to start working\n", p_bag->ID, waitcount);
        pthread_mutex_unlock(p_bag->p_outputMutex);
      }

      waitcount++;

    }

    pthread_mutex_lock(p_bag->p_outputMutex);
    printf("ID %d: got signal to start working\n", p_bag->ID);
    pthread_mutex_unlock(p_bag->p_outputMutex);


    /* Calculate eigenvalues and eigenvectors*/
    for(itr = 0; itr < p_bag->assetNum; itr++){
      poweralgWrapper(p_bag,itr);

      if((p_bag->p_eigval)[itr]/(p_bag->p_eigval)[0] < tolerance){
        pthread_mutex_lock(p_bag->p_outputMutex);
        printf(" ID %d converged to tolerance %g! on job %d\n", ID, tolerance, p_bag->jobNumber); 
        pthread_mutex_unlock(p_bag->p_outputMutex);
        break;
      }

      matrixSubtractionWrapper(p_bag, itr);

    }

    /* Check if there is error in engine. */
    if(engineWrapper(p_bag, itr)) {

      pthread_mutex_lock(p_bag->p_outputMutex);
      printf("ID %d: error in engine.\n", p_bag->ID);
      pthread_mutex_unlock(p_bag->p_outputMutex);

      pthread_mutex_lock(p_bag->p_synchro);
      p_bag->status = ENGINEPROBLEM;
      p_bag->command = STANDBY;
      pthread_mutex_unlock(p_bag->p_synchro);
      
    }
    else {

      pthread_mutex_lock(p_bag->p_synchro);
      p_bag->status = DONEWITHWORK;
      p_bag->command = STANDBY;
      pthread_mutex_unlock(p_bag->p_synchro);
    }
  }

DONE:
  pthread_mutex_lock(p_bag->p_outputMutex);
  printf(" ID %d quitting\n", p_bag->ID);
  pthread_mutex_unlock(p_bag->p_outputMutex);
}
