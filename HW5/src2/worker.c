#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "powerbag.h"

void workerOptimization(PowerBag *p_bag)
{
  int itr, ID, letsgo, waitcount, i;
  double tolerance;


  ID = p_bag->ID;
  tolerance = p_bag->tolerance;

  pthread_mutex_lock(p_bag->output);
  printf("\nID %d: ID %d starts\n", ID, ID);
  pthread_mutex_unlock(p_bag->output);

  for(;;){

    pthread_mutex_lock(p_bag->output);
    printf("\nID %d: ID %d in big loop\n", ID, ID);
    pthread_mutex_unlock(p_bag->output);

    letsgo = 0;
    waitcount = 0;
    while(letsgo == 0){
      /** wait until first WORK signal **/
      sleep(1);

      pthread_mutex_lock(p_bag->synch);
      if(p_bag->command == WORK){
        letsgo = 1;
      }
      else if(p_bag->command == QUIT)
        letsgo = 2;
      pthread_mutex_unlock(p_bag->synch);

      if (letsgo == 2) 
        goto DONE;

      if(0 == waitcount%2){
        pthread_mutex_lock(p_bag->output);
        printf("\nID %d: wait %d for signal to start working\n", ID, waitcount);
        pthread_mutex_unlock(p_bag->output);
      }

      waitcount++;

    }

    pthread_mutex_lock(p_bag->output);
    printf("\nID %d: got signal to start working\n", p_bag->ID);
    pthread_mutex_unlock(p_bag->output);


    /* Calculate eigenvalues and eigenvectors*/
    for(itr = 0; itr < p_bag->numassets; itr++){
      getstatisticsWrapper(p_bag);
      poweralgWrapper(p_bag,itr);
      for (i=0; i<p_bag->numassets; i++){
        p_bag->eigvec[itr*p_bag->numassets+i] = p_bag->tmpvec[i]; 
      }

      if((p_bag->eigval)[itr]/(p_bag->eigval)[0] < tolerance){
        pthread_mutex_lock(p_bag->output);
        printf("\nID %d converged to tolerance %g! on job %d\n", ID, tolerance, p_bag->jobnumber); 
        pthread_mutex_unlock(p_bag->output);
        break;
      }

      matrixSubtractionWrapper(p_bag, itr);

    }

    /* Check if there is error in engine. */
    if(engineWrapper(p_bag, itr)) {

      pthread_mutex_lock(p_bag->output);
      printf("\nID %d: error in engine.\n", p_bag->ID);
      pthread_mutex_unlock(p_bag->output);

      pthread_mutex_lock(p_bag->synch);
      p_bag->status = ENGINEPROBLEM;
      p_bag->command = STANDBY;
      pthread_mutex_unlock(p_bag->synch);
      
    }
    else {

      pthread_mutex_lock(p_bag->synch);
      p_bag->status = DONEWITHWORK;
      p_bag->command = STANDBY;
      pthread_mutex_unlock(p_bag->synch);
    }
  }

DONE:
  pthread_mutex_lock(p_bag->output);
  printf("\nID %d quitting\n", p_bag->ID);
  pthread_mutex_unlock(p_bag->output);
}
