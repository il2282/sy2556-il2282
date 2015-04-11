#ifndef POWER

#define POWER


#define WAITING 100
#define WORKING 101
#define PREANYTHING 102
#define DONEWITHWORK 103


#define QUIT 200
#define WORK 201
#define STANDBY 202

typedef struct PowerBag{
  int rtnNum;
  int assetNum;
  double *p_pertAssetRtn;
  double *p_mean;
  double *p_var;
  double *p_optimal;
  int ID;
  int status;
  int command;
  int jobNumber;  
  pthread_mutex_t *p_synchro;
  pthread_mutex_t *p_outputMutex;
}PowerBag;

void PWRpoweralg_new(powerbag *pbag);
void PWRfreespace(powerbag **ppbag);
void PWRfree(double **pvector);
int PWRreadnload_new(char *filename, int ID, powerbag **ppbag);

int PWRallocatespace(int assetNum, int rtnNum, double **pp_pertAssetRtn, double **pp_mean, double **pp_var, double **pp_optimal);
int PWRreadnload(char *filename, int *pn, double **pvector, double **pnewvector, double **pmatrix);

void PWRpoweriteration(int ID, int k, 
		    int n, double *vector, double *newvector, double *matrix,
		       double *peigvalue, double *perror,
		         pthread_mutex_t *poutputmutex);

void PWRpoweralg(int n, double *vector, double *newvector, double *matrix,
		 double *peigvalue);

void PWRshowvector(int n, double *vector);

void PWRcompute_error(int n, double *perror, double *newvector, double *vector);
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
