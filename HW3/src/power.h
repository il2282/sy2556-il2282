#ifndef POWER

#define POWER

#define NOMEMORY 100

int allocatespace(int n, double **pvector, double **pnewvector, double **pmatrix);
int readnload(char *filename, int *pn, double **pvector, double **pnewvector, double **pmatrix);

void poweriteration(int k, 
		    int n, double *vector, double *newvector, double *matrix,
		    double *perror);

void poweralg(int n, double *vector, double *newvector, double *matrix);

void showvector(int n, double *vector);

void compute_error(int n, double *perror, double *newvector, double *vector);
#endif

/*class powerunit{
 public:
  powerunit(char *inputname);
  ~powerunit();
  int readnload(char *file);
  int allocatespace();
  void iterate();
  void showvector(){ for(int j = 0; j < n; j++) printf("%g ",vector[j]);
  printf("\n");}
  char *getname(){return name;}
 private:
  void releasespace();
  int n;
  double *matrix;
  double *vector;
  double *newvector;
  char *name;
};
*/
