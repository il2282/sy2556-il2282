#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "inputreader.h"


int readdata(char *filename, int numassets, int numperiods, double **p_price, double **p_return){
	int retcode = 0;
	FILE *datafile;
	char buffer[100];
	double *matrix = NULL, *price = NULL;
	double prev, curr;
	int i,j;

	datafile = fopen(filename, "r");
	if (!datafile){
		printf("readdata:: cannot read file %s\n", filename);
		retcode = FILEERROR; goto BACKDATA;
	}
	
	matrix = (double *)calloc(numperiods*numassets + numassets, sizeof(double));
	price = matrix + numassets;

	for (j = 0; j < numassets; j++){
		fscanf(datafile, "%s", buffer);
		prev = atof(buffer);
		price[j] = prev;

		for (i = 0; i < numperiods; i++){
			fscanf(datafile, "%s", buffer);
			curr = atof(buffer);
			
			matrix[j*numperiods+i] = (curr-prev)/prev;
			prev = curr;
		}		
	}


	*p_return = matrix;
	*p_price = price;


BACKDATA:
	if (datafile) fclose(datafile);
	return retcode;	

}


int readparameter(char *filename, int *p_numassets, int *p_numperiods, double *p_initial_investment, int *p_investment_horizon, double *p_risk_aversion, double *p_threshold, double *p_tolerance, double *p_czero){
	int retcode = 0;
	FILE *paramfile;
	char buffer[100];
	int numassets, numperiods;
	double initial_investment = 1.0e9, risk_aversion = 0.0, threshold = 0.1, tolerance = 0.01, czero = 1.0e-6;
	int investment_horizon = 1; 

	paramfile = fopen(filename, "r");
	if (!paramfile){
		printf("readparameter:: cannot read file %s\n", filename);
		retcode = FILEERROR; goto BACKPARAM;
	}

	/* essential parameters */
	fscanf(paramfile, "%s", buffer);
	fscanf(paramfile, "%s", buffer);
	numassets = atoi(buffer);		

	fscanf(paramfile, "%s", buffer);
	fscanf(paramfile, "%s", buffer);
	numperiods = atoi(buffer)-1;		

	/* optional parameters */
	for (;;){
		fscanf(paramfile, "%s", buffer);
		if (0 == strcmp(buffer, "initial_investment")){
			fscanf(paramfile, "%s", buffer);
			initial_investment = atof(buffer);		

		} else if (0 == strcmp(buffer, "investment_horizon")){
			fscanf(paramfile, "%s", buffer);
			investment_horizon = atoi(buffer);

		} else if (0 == strcmp(buffer, "risk_aversion")){
			fscanf(paramfile, "%s", buffer);
			risk_aversion = atof(buffer);		

		} else if (0 == strcmp(buffer, "threshold")){
			fscanf(paramfile, "%s", buffer);
			threshold = atof(buffer);		

		} else if (0 == strcmp(buffer, "tolerance")){
			fscanf(paramfile, "%s", buffer);
			tolerance = atof(buffer);		

		} else if (0 == strcmp(buffer, "zero")){
			fscanf(paramfile, "%s", buffer);
			czero = atof(buffer);		

		} else if (0 == strcmp(buffer, "END")){
			break;

		} else {
			printf("readparameter:: --> wrong format\n");
			retcode = FORMATERROR; goto BACKPARAM;
		}
	}

	*p_numassets = numassets;
	*p_numperiods = numperiods;
	*p_initial_investment = initial_investment;
	*p_investment_horizon = (investment_horizon > numperiods)? numperiods: investment_horizon;
	*p_risk_aversion = risk_aversion;
	*p_threshold = threshold;
	*p_tolerance = tolerance;
	*p_czero = czero;


BACKPARAM:
	if (paramfile) fclose(paramfile);
	return retcode;

}

int readstructure(char *filename, char **constr_filename, char **ruleone_filename, char **ruletwo_filename){
	int retcode = 0;
	FILE *structurefile;
	char buffer[100];

	structurefile = fopen(filename, "r");
	if (!structurefile){
		printf("readstructure:: cannot read file %s\n", filename);
		retcode = FILEERROR; goto BACKSTRUCT;
	}
	for (;;){
		fscanf(structurefile, "%s", buffer);	
		if (0 == strcmp(buffer, "constr")){
			fscanf(structurefile, "%s", buffer);
			*constr_filename = strdup(buffer);

		} else if (0 == strcmp(buffer, "ruleone")){
			fscanf(structurefile, "%s", buffer);
			*ruleone_filename = strdup(buffer);

		} else if (0 == strcmp(buffer, "ruletwo")){
			fscanf(structurefile, "%s", buffer);
			*ruletwo_filename = strdup(buffer);

		} else if (0 == strcmp(buffer, "END")){
			break;

		} else {
			printf("readstructure:: --> wrong format\n");
			retcode = FORMATERROR; goto BACKSTRUCT;
		}
	}
	

BACKSTRUCT:
	if (structurefile) fclose(structurefile);
	return retcode;

}

int readconstr(char *filename, int numassets, double **p_initx, double **p_ub, double **p_lb){
	int retcode = 0;
	double *initx = NULL, *ub = NULL, *lb = NULL;
	FILE *constrfile;
	char buffer[100];
	int j;

	constrfile = fopen(filename, "r");
	if (!constrfile){
		printf("readconstr:: cannot read file %s\n", filename);
		retcode = FILEERROR; goto BACKCONSTR;
	}

	initx = (double *)calloc(3*numassets, sizeof(double));
	if (!initx){
		printf("readconstr:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACKCONSTR;
	}
	ub = initx+numassets;
	lb = initx+2*numassets;

	fscanf(constrfile, "%s", buffer);
	for (j = 0; j < numassets; j++){
		fscanf(constrfile, "%s", buffer);
		initx[j] = atof(buffer);
	}
	
	fscanf(constrfile, "%s", buffer);
	for (j = 0; j < numassets; j++){
		fscanf(constrfile, "%s", buffer);
		ub[j] = atof(buffer);
	}

	fscanf(constrfile, "%s", buffer);
	if (0 == strcmp(buffer, "END")){
		lb = ub;
	} else {
		for (j = 0; j < numassets; j++){
			fscanf(constrfile, "%s", buffer);
			lb[j] = atof(buffer);
		}
	}

	*p_initx = initx;
	*p_ub = ub;
	*p_lb = lb;
	

BACKCONSTR:
	return retcode;
}

int readruleone(char *filename, int *p_numr, RuleOne **p_r){
	int retcode = 0;
	int numr = 0; RuleOne *r = NULL;
	FILE *rulefile;
	char buffer[100];
	int j;

	rulefile = fopen(filename, "r");
	if (!rulefile){
		printf("readruleone:: cannot read file %s\n", filename);
		retcode = FILEERROR; goto BACKRONE;
	}

	fscanf(rulefile, "%s", buffer);
	numr = atoi(buffer);
	printf("readruleone:: %d rule_one(s)\n", numr);

	/* allocate memory */
	r = (RuleOne *)calloc(numr, sizeof(RuleOne));

	for (j = 0; j < numr; j++){
		fscanf(rulefile, "%s", buffer);
		(r+j)->a = atof(buffer);
		fscanf(rulefile, "%s", buffer);
		(r+j)->b = atof(buffer);
		fscanf(rulefile, "%s", buffer);
		(r+j)->N_one = atoi(buffer);
		fscanf(rulefile, "%s", buffer);
		(r+j)->N_two = atoi(buffer);
	}

	fscanf(rulefile, "%s", buffer);
	if (0 != strcmp(buffer, "END")){
		printf("readruleone:: --> no END\n");
		retcode = FORMATERROR; goto BACKRONE;
	}

	*p_numr = numr;
	*p_r = r;

BACKRONE:
	if (rulefile) fclose(rulefile);
	return retcode;

}

int readruletwo(char *filename, int *p_numr, RuleTwo **p_r){
	int retcode = 0;
	int numr = 0; RuleTwo *r = NULL;
	FILE *rulefile;
	char buffer[100];
	int j, i;

	rulefile = fopen(filename, "r");
	if (!rulefile){
		printf("readruletwo:: cannot read file %s\n", filename);
		retcode = FILEERROR; goto BACKRTWO;
	}

	fscanf(rulefile, "%s", buffer);
	numr = atoi(buffer);
	printf("readruletwo:: %d rule_two(s)\n", numr);

	/* allocate memory */
	r = (RuleTwo *)calloc(numr, sizeof(RuleTwo));

	for (j = 0; j < numr; j++){
		fscanf(rulefile, "%s", buffer);
		(r+j)->numindices = atoi(buffer);

		allocateruletwoindices(&r[j]);
		for (i = 0; i < (r+j)->numindices; i++){
			fscanf(rulefile, "%s", buffer);
			((r+j)->indices)[i] = atoi(buffer);	
		}
		
		fscanf(rulefile, "%s", buffer);
		(r+j)->delta = atof(buffer);
		fscanf(rulefile, "%s", buffer);
		(r+j)->direction = (0 == strcmp(buffer, "L"))? LONG : SHORT;
	}

	fscanf(rulefile, "%s", buffer);
	if (0 != strcmp(buffer, "END")){
		printf("readruletwo:: --> no END\n");
		retcode = FORMATERROR; goto BACKRTWO;
	}

	*p_numr = numr;
	*p_r = r;

BACKRTWO:
	if (rulefile) fclose(rulefile);
	return retcode;

}
