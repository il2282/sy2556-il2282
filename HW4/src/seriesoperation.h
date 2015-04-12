#ifndef SERIESOP

#define SERIESOP

#include <stdio.h>
#include <stdlib.h>
#include "powerbag.h"

int readAssetObsGetMean(char *p_obsFile, double **pp_assetRtn, double **pp_mean, int assetNum, int rtnNum);
int meanAllocateSpace(double **pp_assetRtn, double **pp_mean, int assetNum, int rtnNum);
void mean(double* p_assetObs, double *p_mean, int assetNum, int rtnNum);
int timeSeriesPerturb(double *p_assetRtn, PowerBag* p_bag, double *v, double epsSd, double orgProp);
void var(double *p_assetobs, double *p_var, double *p_mean, int assetnum, int obsnum);

#endif