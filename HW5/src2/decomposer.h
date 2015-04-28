#ifndef DECOMPOSER

#define DECOMPOSER

#define NOMEMORY 10

int getstdev(int numassets, int numperiods, double *matrix, double **p_mean, double **p_stdev);

int getstatistics(int numassets, int numperiods, double *matrix, double **p_mean, double **p_cov);

int perturbreturns(int numassets, int numperiods, double *assetreturns, double *stdev, double **p_perturbed);

#endif