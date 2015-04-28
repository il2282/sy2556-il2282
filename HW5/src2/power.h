#ifndef POWER

#define POWER

void poweriteration(int n, double *vector, double* newvector, double *matrix, double *p_error, double *p_val);

void poweralg(int n, double *matrix, double *p_vector, double *p_val, double tolerance);

void compute_error(int n, double *p_error, double *newvector, double *vector);

void matrix_subtraction(int n, double *matrix, double *p_eigvec, double eigval);
#endif