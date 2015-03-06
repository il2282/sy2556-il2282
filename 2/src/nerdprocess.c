#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void gotosleep(int numseconds);
char does_it_exist(char *filename);

int writelp(double *tomatch, char *lpfilename, int nerdID, int T,
  double blimit, double brate, int cpterm, double cprate, double exrate);
int solverinteraction(double *tomatch, double *pfinalcashflow, int nerdID, int T, 
  double blimit, double brate, int cpterm, double cprate, double exrate);
double drawnormal(void);

int processfile(char *constfilename, char *filename, double *pfinalcashflow, int nerdID)
{
  int code = 0;
  char buffer[100];
  double blimit, brate, cprate, exrate;
  int cpterm;
  int T;
  double w;
  double *tomatch = NULL;
  FILE *theconst = NULL, *theflows = NULL;
  int j;

  theconst = fopen(constfilename, "r");
  if (!theconst){
    printf("cannot open %s\n", constfilename); code = 100; 
    goto OUT;
  }

  fscanf(theconst,"%s",buffer);
  fscanf(theconst,"%s",buffer);
  blimit = atof(buffer);
  fscanf(theconst,"%s",buffer);
  fscanf(theconst,"%s",buffer);
  brate = 1+atof(buffer)/100;
  fscanf(theconst,"%s",buffer);
  fscanf(theconst,"%s",buffer);
  cpterm = atoi(buffer);
  fscanf(theconst,"%s",buffer);
  fscanf(theconst,"%s",buffer);
  cprate = 1+atof(buffer)/100;
  fscanf(theconst,"%s",buffer);
  fscanf(theconst,"%s",buffer);
  exrate = 1+atof(buffer)/100;

  fscanf(theconst,"%s",buffer);  
  if(0 != strcmp(buffer, "END")){
    printf("warning:  no 'END'\n");
  }
  fclose(theconst);
  printf("blimit = %g brate = %g cpterm = %d cprate = %g exrate = %g\n", 
    blimit, brate, cpterm, cprate, exrate);


  theflows = fopen(filename, "r");
  if (!theflows){
    printf("cannot open %s\n", filename); code = 100; 
    goto OUT;
  }

  fscanf(theflows,"%s",buffer);
  T = atoi(buffer);
  fscanf(theflows,"%s",buffer); /* the word 'periods', which we ignore **/

  printf("T = %d\n", T);
  if( (T <= 0) ){
    printf("illegal T\n"); code = 200; fclose(theflows); goto OUT;
  }

  tomatch = (double *)calloc(T, sizeof(double));
  if(!tomatch){
    printf("no memory\n"); code = 300; goto OUT;
  }

  w = drawnormal();

  for(j = 0; j < T; j++){
    fscanf(theflows,"%s",buffer); 
    tomatch[j] = atof(buffer);
    fscanf(theflows,"%s",buffer);
    tomatch[j] += w*atof(buffer);
  }

  fscanf(theflows,"%s",buffer);  
  if(0 != strcmp(buffer, "END")){
    printf("warning:  no 'END'\n");
  }
  
  fclose(theflows);

  code = solverinteraction(tomatch, pfinalcashflow, nerdID, T, 
    blimit, brate, cpterm, cprate, exrate);
  if(code) goto OUT;

 OUT:

  if(tomatch) 
    free(tomatch); /** must free **/

  return code;
}

int solverinteraction(double *tomatch, double *pfinalcashflow, int nerdID, int T, 
  double blimit, double brate, int cpterm, double cprate, double exrate)
{
  int solvecode = 0, k, numnonz;
  char commandbuffer[200];
  char readbuffer[300], otherbuffer[100];
  char hiddenfilename[100], nothiddenfilename[100];
  char LPfilename[100];
  char gurobipylogname[100];
  FILE *results;
  FILE *out;
  double optimalvalue, solvalue;


  sprintf(LPfilename,"./lp/cash_%d.lp",nerdID);
  sprintf(hiddenfilename,"./tmp/hidden_%d",nerdID);
  sprintf(nothiddenfilename,"./tmp/nothidden_%d",nerdID);
  sprintf(gurobipylogname,"./lp/mygurobi_%d",nerdID);

  solvecode = writelp(tomatch, LPfilename, nerdID, T, blimit, brate, cpterm, cprate, exrate);
  
  if(solvecode) goto BACK;



  /** first, cleanup **/
  if (does_it_exist(hiddenfilename)){
	  remove(hiddenfilename);
  }
  if (does_it_exist(nothiddenfilename)){
	  remove(nothiddenfilename);
  }

  /** next create the hidden file if necessary **/
  if (does_it_exist(hiddenfilename) == 0){
    out = fopen(hiddenfilename, "w");
    fclose(out);
  }


  sprintf(commandbuffer, "python ./src/gurobiwithnerd.py %s %s %s %d", 
	  LPfilename, hiddenfilename, nothiddenfilename, nerdID);

  system(commandbuffer);
/** sleep-wake cycle **/

  for (;;){
    if (does_it_exist(nothiddenfilename)){ 
      printf("\ngurobi done!\n");
      gotosleep(1);
      break;
    }
    else{
      gotosleep(1);
    }
  }


  results = fopen(gurobipylogname, "r");
  if (!results){
    printf("cannot open %s\n", gurobipylogname); solvecode = 111; 
    goto BACK;
  }
  /* read until finding Optimal or infeasible or unbounded */

  for (;;){
	  fscanf(results, "%s", readbuffer);
	  /* compare readbuffer to 'Optimal'*/
	  if (strcmp(readbuffer, "Optimal") == 0){
		  /* now read three more*/
		  fscanf(results, "%s", readbuffer);
		  fscanf(results, "%s", readbuffer);
		  fscanf(results, "%s", readbuffer);
		  optimalvalue = atof(readbuffer);
		  printf(" value = %g\n", optimalvalue);
		  if (optimalvalue > .0001){
			  printf("making some money!\n");
			  /* read again to get the number of nonzeros*/
			  fscanf(results, "%s", readbuffer);
			  numnonz = atoi(readbuffer);
			  fscanf(results, "%s", readbuffer); fscanf(results, "%s", readbuffer); fscanf(results, "%s", readbuffer);
			  fscanf(results, "%s", readbuffer);
			  for (k = 0; k < numnonz; k++){
			    fscanf(results, "%s", otherbuffer);
			    fscanf(results, "%s", readbuffer); fscanf(results, "%s", readbuffer);
				  solvalue = atof(readbuffer);
				  printf("%s -> %g\n", otherbuffer, solvalue);
			  }
		  }
		  else{
		    printf("lost money ... :(\n");
		    break;
		  }
	  }
	  else if ((strcmp(readbuffer, "infeasible") == 0) ||
		   strcmp(readbuffer, "unbounded") == 0){
	    printf(" ==>> LP infeasible or unbounded\n");
	    break;
	  }
	  else if (strcmp(readbuffer, "bye.") == 0){
		  break;
	  }
  }
   
  fclose(results);
 BACK:
  *pfinalcashflow = optimalvalue;
  return solvecode;
}

int writelp(double *tomatch, char *lpfilename, int nerdID, int T,
  double blimit, double brate, int cpterm, double cprate, double exrate)
{
  int writecode = 0;
  FILE *LPfile;
  int t;
  /*double rplus1;
  double drawn;

  drawn = rand()/((double) RAND_MAX);


  drawn = 0.5*(1 + drawnormal());
  if (drawn < 0) 
    drawn = 0;

  printf("nerd %d draws %g so rate = %g\n", nerdID, drawn, r*drawn);

  rplus1 = 1 + r*drawn;*/
  
  LPfile = fopen(lpfilename, "w");
  if(!LPfile){
    printf("cannot open file %s\n", lpfilename); 
    writecode = 155; goto BACK;
  }
  printf("\nWriting LP to file %s\n", lpfilename);

  fprintf(LPfile,"Maximize v%d\nSubject to\n", T);
  
  /** first period is special **/
  fprintf(LPfile,"p1: x1 + y1 - z1 = %g\n", -tomatch[0]);

  /** next, all but the very last period **/

  
  for(t = 2; t <= T; t++){
    fprintf(LPfile,"p%d:", t);
    if(t < T) fprintf(LPfile,"x%d", t);
    if(t <= T-cpterm) fprintf(LPfile," + y%d", t);
    if(t > cpterm){
          fprintf(LPfile," - %g y%d", cprate, t-cpterm);
    }
    if(t < T) fprintf(LPfile," - %g x%d + %g z%d - z%d = %g\n", brate, t-1, exrate, t-1,t,-tomatch[t-1]); 

    if(t == T) fprintf(LPfile," - %g x%d + %g z%d - v%d = %g\n", brate, t-1, exrate, t-1,T,-tomatch[t-1]);
    
  }

  /** bounds **/
  fprintf(LPfile,"Bounds\n");
  for(t = 1; t< T; t++){
    fprintf(LPfile,"x%d <= %g\n", t, blimit);
  }
  fprintf(LPfile,"v%d free\n", T);
  

  fprintf(LPfile,"END\n");
  fclose(LPfile);

 BACK:
  return writecode;
}



/*

  drawn = 0.5*(1 + drawnormal());
  if (drawn < 0) 
    drawn = 0;

*/
