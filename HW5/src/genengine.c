
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <gurobi_c.h>
#include <math.h>

int engine(int numassets, int numfactors, 
	     double *ub, double *lb, double *mu, double *sigma2, 
	   double *V, double *F, int max_names, double *theta,
	   double *initx, 
	   char *paramsfilename)
{
  int retcode = 0;
  GRBenv   *env = NULL, *modelenv;
  GRBmodel *model = NULL;
  int n, i, j, k;
  double *x;
  int *qrow, *qcol, Nq;
  double *qval;
  int *cind;
  double rhs;
  char sense;
  double *cval;
  int numnonz;
  int gurobierror;
  char **names, bigname[100], buffer[100], *vartype;
  double buffervalue;
  FILE *input;

  printf("running solver engine\n");

  n = 4*numassets + numfactors;

  retcode = GRBloadenv(&env, "factors_mip.log");
  if (retcode) goto BACK;

 /* Create initial model */
  retcode = GRBnewmodel(env, &model, "factor", n, 
                      NULL, NULL, NULL, NULL, NULL);
  if (retcode) goto BACK;

  names = (char **) calloc(n, sizeof(char *));

  /** next we create the remaining attributes for the n columns **/
  x     = (double *) calloc (n, sizeof(double));
  vartype = (char *)calloc(n, sizeof(char));

  for(j = 0; j < numassets; j++){
    names[j] = (char *)calloc(3, sizeof(char));
    if(names[j] == NULL){
      retcode = 1; goto BACK;
    }
    sprintf(names[j],"x%d", j);
  }
  for(j = numassets; j < numassets + numfactors; j++){
    names[j] = (char *)calloc(3, sizeof(char));
    if(names[j] == NULL){
		  retcode = 1; goto BACK;
    }
    sprintf(names[j],"F%d", j - numassets);
  }

  for(j = numassets + numfactors; j < 2*numassets + numfactors; j++){
    names[j] = (char *)calloc(3, sizeof(char));
    if(names[j] == NULL){
		  retcode = 1; goto BACK;
    }
    sprintf(names[j],"y%d", j - numassets - numfactors);
    ub[j] = 1; lb[j] = 0; 
  }

  for(j = 2*numassets + numfactors; j < 3*numassets + numfactors; j++){
    names[j] = (char *)calloc(6, sizeof(char));
    if(names[j] == NULL){
		  retcode = 1; goto BACK;
    }
    sprintf(names[j],"deltap%d", j - 2*numassets - numfactors);
    ub[j] = 2; lb[j] = 0; 
  }
  for(j = 3*numassets + numfactors; j < 4*numassets + numfactors; j++){
    names[j] = (char *)calloc(6, sizeof(char));
    if(names[j] == NULL){
		  retcode = 1; goto BACK;
    }
    sprintf(names[j],"deltam%d", j - 3*numassets - numfactors);
    ub[j] = 2; lb[j] = 0; 
  }

  /* initialize variables */
  for(j = 0; j < n; j++){
    retcode = GRBsetstrattrelement(model, "VarName", j, names[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "Obj", j, -mu[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "LB", j, lb[j]);
    if (retcode) goto BACK;

    retcode = GRBsetdblattrelement(model, "UB", j, ub[j]);
    if (retcode) goto BACK;

    if ( (j < numassets + numfactors) || (j >= 2*numassets + numfactors) ) vartype[j] = GRB_CONTINUOUS;
    else vartype[j] = GRB_BINARY;
	
    retcode = GRBsetcharattrelement (model, "VTYPE", j, vartype[j]);
    if (retcode) goto BACK;

  }

  /** next, the quadratic -- there are numassets + numfactors*numfactors nonzeroes: 
      numassets residual variances plus the numfactors x numfactors
      factor covariance matrix**/

  Nq = numassets + numfactors*numfactors;
  qrow = (int *) calloc(Nq, sizeof(int));  /** row indices **/
  qcol = (int *) calloc(Nq, sizeof(int));  /** column indices **/
  qval = (double *) calloc(Nq, sizeof(double));  /** values **/

  if( ( qrow == NULL) || ( qcol == NULL) || (qval == NULL) ){
    printf("could not create quadratic\n");
    retcode = 1; goto BACK;
  }

  for (j = 0; j < numassets; j++){
    qval[j] = sigma2[j];
    qrow[j] = qcol[j] = j;
  }
  for (i = 0; i < numfactors; i++){
    for (j = 0; j < numfactors; j++){
      k = i*numfactors + j;
      qval[k + numassets] = F[k];
      qrow[k + numassets] = numassets + i;
      qcol[k + numassets] = numassets + j;
    }
  }
  retcode = GRBaddqpterms(model, Nq, qrow, qcol, qval);
  if (retcode) goto BACK;

  /** now we will add one constraint at a time **/
  /** we need to have a couple of auxiliary arrays **/

  cind = (int *)calloc(n, sizeof(int));  /** n is over the top since no constraint is totally dense;		     but it's not too bad here **/
  cval= (double *)calloc(n, sizeof(double));
  if(!cval){
    printf("cannot allocate cval\n"); retcode = 2; goto BACK;
  }
  for(i = 0; i < numfactors; i++){
    for(j = 0; j < numassets; j++){
      cval[j] = V[i*numassets + j];
      cind[j] = j;
    }
    cind[numassets] = /* j */ numassets + i;
    cval[numassets] = -1;
    numnonz = numassets + 1;
    rhs = 0;
    sense = GRB_EQUAL;

    sprintf(bigname,"factor%d",i);
    retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, bigname);
    if (retcode) goto BACK;

  }

  /** sum of x variables = 1 not used any more**/


  for (j = 0; j < numassets; j++){
    cval[0] = 1.0;  cind[0] = j;
    cval[1] = -ub[j];  cind[1] = numassets + numfactors + j;

    numnonz = 2;
    rhs = 0.0;
    sense = GRB_LESS_EQUAL;

    /* let's reuse some space */
    sprintf(bigname, "control%d", j);

    retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, bigname);
    if (retcode) goto BACK;

    cval[0] = -1.0;  cind[0] = j;
    cval[1] = -ub[j];  cind[1] = numassets + numfactors + j;

    numnonz = 2;
    rhs = 0.0;
    sense = GRB_LESS_EQUAL;

    /* let's reuse some space */
    sprintf(bigname, "Kontrol%d", j);

    retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, bigname);
    if (retcode) goto BACK;

    if(theta && (theta[j] > 0)){
      /** add lower lim constraint **/
      cval[0] = -1.0;  cind[0] = j;
      cval[1] = theta[j];  cind[1] = numassets + numfactors + j;

      numnonz = 2;
      rhs = 0.0;
      sense = GRB_LESS_EQUAL;

      sprintf(bigname, "lower_if_used_%d", j);

      retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, bigname);
      if (retcode) goto BACK;

    }
  }

  /** cardinality constraint **/
  for(j = 0; j < numassets ; j++){ 
    cval[j] = 1.0;
    cind[j] = j + numassets + numfactors;
  }

  numnonz = numassets;
  rhs = (double) max_names;
  sense = GRB_LESS_EQUAL;

  retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, "maxnames");
  if (retcode) goto BACK;

  for (j = 0; j < numassets; j++){
    cval[0] = 1.0;  cind[0] = j;
    cval[1] = -1;  cind[1] = 2*numassets + numfactors + j;
    cval[2] = 1;  cind[2] = 3*numassets + numfactors + j;

    numnonz = 3;
    rhs = (initx) ? initx[j] : 0;
    sense = GRB_EQUAL;

    sprintf(bigname,"balance%d",j);

    retcode = GRBaddconstr(model, numnonz, cind, cval, sense, rhs, bigname);
    if (retcode) goto BACK;

  }



  retcode = GRBupdatemodel(model);
  if (retcode) goto BACK;

  /** optional: write the problem **/

  retcode = GRBwrite(model, "enginemip.lp");
  if (retcode) goto BACK;

  if(paramsfilename != NULL){

    modelenv = GRBgetenv(model);
    if (!modelenv) {
      printf("Cannot retrieve model environment\n");
      retcode = 1; goto BACK;
    }

    printf("reading gurobi params file %s\n", paramsfilename);
    if(!(input = fopen(paramsfilename,"r"))){
      printf("cannot open it\n"); retcode = 1; goto BACK;
    }
    for(;;){
      fscanf(input,"%s", bigname);
      if(0 == strcmp(bigname, "END"))
	break;
      fscanf(input,"%s", buffer);
      buffervalue = atof(buffer);
      printf("**setting '%s' to %g\n", bigname, buffervalue);

      /** now do it **/
      gurobierror = GRBsetdblparam(modelenv, bigname, buffervalue);
      if(gurobierror){
	printf("cannot set; error code %d\n", gurobierror);
	retcode = 1; goto BACK;
      }
    }
    fclose(input);
  }


  /** OLD CODE let's add some early termination options 


  gurobierror = GRBsetdblparam(modelenv, "TimeLimit", 30000);
  if(gurobierror){
    printf("cannot set timelimit; error code %d\n", gurobierror);
    retcode = 1; goto BACK;
  }


  gurobierror = GRBsetdblparam(modelenv, "NodeLimit", 50000.);
  if (gurobierror) goto BACK;


  gurobierror = GRBsetdblparam(modelenv, "MIPGap", 0.03);
  if (gurobierror) goto BACK;

  **/


  retcode = GRBoptimize(model);
  if (retcode) goto BACK;


  /** get solution **/


  retcode = GRBgetdblattrarray(model,
                               GRB_DBL_ATTR_X, 0, n,
                               x);
  if(retcode) goto BACK;

  /** now let's see the values **/

  for(j = 0; j < n; j++){
    if( (names[j][0] == 'x') && (fabs(x[j]) > 1.0e-05))
      printf("%s = %g\n", names[j], x[j]);
  }

  GRBfreemodel(model);
  GRBfreeenv(env);



 BACK:
  printf("engine exits with code %d\n", retcode);
  return retcode;
}
