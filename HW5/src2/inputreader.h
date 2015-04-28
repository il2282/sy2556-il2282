#ifndef INPUTREADER

#include "rulebag.h"

#define INPUTREADER

#define FILEERROR 11
#define FORMATERROR 12

int readdata(char *filename, int numassets, int numperiods, double **p_price, double **p_return);
int readparameter(char *filename, int *p_numassets, int *p_numperiods, double *p_initial_investment, int *p_investment_horizon, double *p_risk_aversion, double *p_threshold, double *p_tolerance, double *p_czero);

int readstructure(char *filename, char **initx_filename, char **ruleone_filename, char **ruletwo_filename);
int readconstr(char *filename, int numassets, double **p_initx, double **p_ub, double **p_lb);
int readruleone(char *filename, int *p_numr, RuleOne **p_r);
int readruletwo(char *filename, int *p_numr, RuleTwo **p_r);

#endif
