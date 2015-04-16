#ifndef SERIESOP

#define SERIESOP

#include <stdio.h>
#include <stdlib.h>


int readAssetObsGetMean(char *p_obsFile, double *p_assetRtn, double *p_mean, int assetNum, int rtnNum);
int timeSeriesPerturb(double *p_assetRtn, PowerBag* p_bag, double *v, double epsSd, double orgProp);
void mean(double* p_assetObs, double *p_mean, int assetNum, int rtnNum);
void var(double *p_assetobs, double *p_var, double *p_mean, int assetnum, int obsnum);

#endif