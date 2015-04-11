#ifndef SERIESOP

#define SERIESOP

#include <stdio.h>
#include <stdlib.h>

int readAssetObsGetMean(char *p_obsFile, double **pp_assetRtn, double **pp_mean, int assetNum, int rtnNum);
int meanAllocateSpace(double **pp_assetRtn, double **pp_mean, int assetNum, int rtnNum);
void mean(double* p_assetObs, double *p_mean, int assetNum, int rtnNum);
int timeSeriesPerturb(int assetNum, int rtnNum, double *p_assetRtn, double *p_mean, double *p_pertAssetRtn, double *v, double epsSd);


#endif