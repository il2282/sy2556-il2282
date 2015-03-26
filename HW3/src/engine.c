
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <gurobi_c.h>


int engine(int numassets, int numeigval, double *mu, double *eigvec, double *eigval, double lambda)
{
  int retcode = 0;
  GRBenv   *env = NULL;
  GRBmodel *model = NULL;
  int n, i, j;
  double *x;
  int *qrow, *qcol;
  double *qval;
  int *cind;
  double rhs;
  char sense;
  double *cval;
  int numnonz;

  char **names, bigname[100];

  printf("running solver engine\n");
  printf("lambda = %f\n", lambda);

  n = numassets + numeigval;

  retcode = GRBloadenv(&env, "meanvar.log");
  if (retcode) goto BACK;

 /* Create initial model */
  retcode = GRBnewmodel(env, &model, "meanvar", n, 
                      NULL, NULL, NULL, NULL, NULL);
  if (retcode) goto BACK;

  names = (char **)calloc(n, sizeof(char *));

  /** next we create the remaining attributes for the n columns **/
  x = (double *)calloc(n, sizeof(double));

  for(j = 0; j < numassets; j++){
    names[j] = (char *)calloc(3, sizeof(char));
    if(names[j] == NULL){
      retcode = 1; goto BACK;
    }
    sprintf(names[j],"x%d", j);
  }
  for(j = numassets; j < n; j++){
    names[j] = (char *)calloc(3, sizeof(char));
    if(names[j] == NULL){
		  retcode = 1; goto BACK;
    }
    sprintf(names[j],"y%d", j - numassets);
  }
  /* initialize variables */
  for(j = 0; j < n; j++){
    retcode = GRBsetstrattrelement(model, "VarName", j, names[j]);
    if (retcode) goto BACK;

    if (j < numassets){
      retcode = GRBsetdblattrelement(model, "Obj", j, -mu[j]);
      if (retcode) goto BACK;
    }

    if (j>=numassets){
      retcode = GRBsetdblattrelement(model, "LB", j, -10000.0);
      if (retcode) goto BACK;

      retcode = GRBsetdblattrelement(model, "UB", j, 10000.0);
      if (retcode) goto BACK;
    }
  }

  /** next, the quadratic -- there are numassets + numfactors*numfactors nonzeroes: 
      numassets residual variances plus the numfactors x numfactors
      factor covariance matrix**/

  qrow = (int *) calloc(numeigval, sizeof(int));  /** row indices **/
  qcol = (int *) calloc(numeigval, sizeof(int));  /** column indices **/
  qval = (double *) calloc(numeigval, sizeof(double));  /** values **/

  if( ( qrow == NULL) || ( qcol == NULL) || (qval == NULL) ){
    printf("could not create quadratic\n");
    retcode = 1; goto BACK;
  }

  for (j = 0; j < numeigval; j++){
    qval[j] = lambda*eigval[j];
    qrow[j] = qcol[j] = numassets+j;
  }

  retcode = GRBaddqpterms(model, numeigval, qrow, qcol, qval);
  if (retcode) goto BACK;

  /** now we will add one constraint at a time **/
  /** we need to have a couple of auxiliary arrays **/

  cind = (int *)calloc(numassets+1, sizeof(int));  /** n is over the top since no constraint is totally dense;		     but it's not too bad here **/
  if(!cind){
    printf("cannot allocate cind\n"); retcode = 2; goto BACK;
  }
  cval= (double *)calloc(numassets+1, sizeof(double));
  if(!cval){
    printf("cannot allocate cval\n"); retcode = 2; goto BACK;
  }

  for(i = 0; i < numassets; i++){
    cval[i] = 1; 
    cind[i] = i;
  }
  sprintf(bigname,"sum");
  retcode = GRBaddconstr(model, numassets, cind, cval, GRB_EQUAL, 1, bigname);
  if (retcode) goto BACK;

  for(i = 0; i < numeigval; i++){
    for(j = 0; j < numassets; j++){
      cval[j] = eigvec[i*numassets + j];
      cind[j] = j;
    }
    cind[numassets] = numassets + i;
    cval[numassets] = -1;
    numnonz = numassets + 1;
    rhs = 0;
    sense = GRB_EQUAL;

    sprintf(bigname,"eigenvector%d",i);
    retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, bigname);
    if (retcode) goto BACK;

  }

  

  retcode = GRBupdatemodel(model);
  if (retcode) goto BACK;

  /** optional: write the problem **/

  retcode = GRBwrite(model, "menavar.lp");
  if (retcode) goto BACK;

  retcode = GRBoptimize(model);
  if (retcode) goto BACK;


  /** get solution **/


  retcode = GRBgetdblattrarray(model,
                               GRB_DBL_ATTR_X, 0, n,
                               x);
  if(retcode) goto BACK;

  /** now let's see the values **/

  printf("Non-zero optimal values:\n");
  for(j = 0; j < n; j++){
    if (x[j]>0.00000000001) printf("%s = %g\n", names[j], x[j]);
  }

  GRBfreemodel(model);
  GRBfreeenv(env);



 BACK:
  printf("engine exits with code %d\n", retcode);
  return retcode;
}