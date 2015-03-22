#ifndef MEANVAR

#define MEANVAR

#include <stdio.h>
#include <stdlib.h>

int read_asset_obs_get_meanvar(char *p_obsfile, double **pp_assetobs, double **pp_mean, double **pp_var, int assetnum, int obsnum);
int meanvar_allocate_space(double **pp_assetobs, double **pp_mean, double **pp_var, int assetnum, int obsnum);
void mean(double *p_assetobs, double *p_mean, int assetnum, int obsnum);
void var(double *p_assetobs, double *p_var, double *p_mean, int assetnum, int obsnum);


#endif