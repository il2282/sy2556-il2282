#include "meanvar.h"

int read_asset_obs_get_meanvar(char* p_obsfile, double **pp_assetobs, double **pp_mean, double **pp_var, int assetnum, int obsnum){
	
	int assetindex, obsindex, readcode = 0;
	FILE* input = NULL;
	char buffer[100];


	readcode = meanvar_allocate_space(pp_assetobs, pp_mean, pp_var, assetnum, obsnum);
	if(readcode) goto BACK;


	input = fopen(p_obsfile, "r");
	if (input == NULL){
		printf("cannot read %s\n", p_obsfile);
		readcode = 1; goto BACK;
	}

	for (assetindex = 0; assetindex < assetnum; assetindex++){
		for (obsindex = 0; obsindex < obsnum; obsindex++){
			fscanf(input, "%s", buffer);
			(*pp_assetobs)[assetindex*obsnum + obsindex] = atof(buffer);
		}
	}
	fclose(input);

	mean(*pp_assetobs, *pp_mean, assetnum, obsnum);

	var(*pp_assetobs, *pp_var, *pp_mean, assetnum, obsnum);

	return readcode;

	BACK:
		return readcode;
}

int meanvar_allocate_space(double **pp_assetobs, double **pp_mean, double **pp_var, int assetnum, int obsnum){
	
	int code = 0;
	double *mem = NULL;

	mem = (double *)calloc(assetnum*obsnum + assetnum + assetnum*assetnum, sizeof(double));
	if (!mem){
		printf("cannot allocate memory.\n");
		code = 1; goto BACK;
	}

	*pp_assetobs = mem;
	*pp_mean = mem+assetnum*obsnum;
	*pp_var = mem+assetnum*obsnum + assetnum;

	return code;

	BACK:
		return code;
}

void mean(double* p_assetobs, double *p_mean, int assetnum, int obsnum){
	
	int assetindex, obsindex;

	for (assetindex = 0; assetindex < assetnum; assetindex++){
		for (obsindex = 0; obsindex < obsnum; obsindex++){
			p_mean[assetindex] += p_assetobs[assetindex*obsnum + obsindex];
		}
		p_mean[assetindex] /= obsnum;
	}

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