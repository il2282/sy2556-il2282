#include "powerbag.h"
#include "seriesoperation.h"
#include "utilities.h"


int readAssetObsGetMean(char *p_obsFile, double **pp_assetRtn, double **pp_mean, int assetNum, int rtnNum){

	int assetIndex, obsIndex, readCode = 0;
	FILE* input = NULL;
	char buffer[100];
	double previousObs, currentObs;


	readCode = meanAllocateSpace(pp_assetRtn, pp_mean, assetNum, rtnNum);
	if(readCode) goto BACK;


	input = fopen(p_obsFile, "r");
	if (input == NULL){
		printf("cannot read %s\n", p_obsFile);
		readCode = 1; goto BACK;
	}


	for (assetIndex = 0; assetIndex < assetNum; assetIndex++){
		fscanf(input, "%s", buffer);
		previousObs = atoi(buffer);
		for (obsIndex = 0; obsIndex < rtnNum; obsIndex++){
			fscanf(input, "%s", buffer);
			currentObs = atoi(buffer);
			(*pp_assetRtn)[assetIndex*rtnNum + obsIndex] = (currentObs - previousObs)/previousObs;
		}
	}
	fclose(input);

	mean(*pp_assetRtn, *pp_mean, assetNum, rtnNum);

	return readCode;

	BACK:
		return readCode;
}

int meanAllocateSpace(double **pp_assetRtn, double **pp_mean, int assetNum, int rtnNum){
	
	int code = 0;
	double *mem = NULL;

	mem = (double *)calloc(assetNum*rtnNum + assetNum, sizeof(double));
	if (!mem){
		printf("cannot allocate memory.\n");
		code = 1; goto BACK;
	}

	*pp_assetRtn = mem;
	*pp_mean = mem+assetNum*rtnNum;

	return code;

	BACK:
		return code;
}

void mean(double* p_assetObs, double *p_mean, int assetNum, int rtnNum){
	
	int assetIndex, obsIndex;

	for (assetIndex = 0; assetIndex < assetNum; assetIndex++){
		for (obsIndex = 0; obsIndex < rtnNum; obsIndex++){
			p_mean[assetIndex] += p_assetObs[assetIndex*rtnNum + obsIndex];
		}
		p_mean[assetIndex] /= rtnNum;
	}

}

int timeSeriesPerturb(double *p_assetRtn, PowerBag* p_bag, double *v, double epsSd, double orgProp)
{
  int retcode = 0, assetIndex, rtnIndex, assetNum, rtnNum;
  double *eps;
  double sum;

  assetNum = p_bag->assetNum;
  rtnNum = p_bag->rtnNum;

  eps = (double *)calloc(rtnNum, sizeof(double));
  if (!eps){
    retcode = NOMEMORY; goto BACK;
  }

  sum = drawNormalVector(eps, rtnNum-1, epsSd);
  eps[rtnNum-1] = -sum;

  for (assetIndex = 0; assetIndex < assetNum; assetIndex++){
    for (rtnIndex = 0; rtnIndex < rtnNum; rtnIndex++){
    	p_bag->p_pertAssetRtn[assetIndex*rtnNum+rtnIndex] = orgProp*p_assetRtn[assetIndex*rtnNum+rtnIndex] + (1-orgProp)*(p_bag->p_mean[assetIndex] + v[assetIndex]*eps[rtnIndex]);
    }
  }

  var(p_bag->p_pertAssetRtn, p_bag->p_var, p_bag->p_mean, assetNum, rtnNum);

  

  BACK:
  	if(eps) free(eps);
    return retcode;

}


void var(double *p_assetobs, double *p_var, double *p_mean, int assetnum, int obsnum){
	
	int assetindex_r, assetindex_c, obsindex;

	for (assetindex_r = 0; assetindex_r < assetnum; assetindex_r++){
		for (assetindex_c = 0; assetindex_c < assetnum; assetindex_c++){
			if (assetindex_c >= assetindex_r){
				for (obsindex = 0; obsindex < obsnum; obsindex++){
					p_var[assetindex_r*assetnum+assetindex_c] += p_assetobs[assetindex_r*obsnum + obsindex]*p_assetobs[assetindex_c*obsnum + obsindex];
				}
				p_var[assetindex_r*assetnum+assetindex_c] /= obsnum;
				p_var[assetindex_r*assetnum+assetindex_c] -= p_mean[assetindex_r]*p_mean[assetindex_c];
			}
			else p_var[assetindex_r*assetnum+assetindex_c] = p_var[assetindex_c*assetnum+assetindex_r];
		}
	}
}