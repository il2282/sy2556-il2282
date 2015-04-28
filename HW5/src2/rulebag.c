#include <stdio.h>
#include <stdlib.h>
#include "rulebag.h"

int allocateruletwoindices(RuleTwo *p_rule){
	int retcode = 0;
	int *indices = NULL;
	int numindices;

	numindices = p_rule->numindices;

	indices = (int *)calloc(numindices, sizeof(int));
	if (!indices){
		printf("allocateruletwoindices:: NO MEMORY\n");
		retcode = NOMEMORY; goto BACK;
	}

	p_rule->indices = indices;

BACK:
	return retcode;
}

void freeruletwoindices(RuleTwo *p_rule){
	if (p_rule->indices) free(p_rule->indices);
}

void makegrbruletwo(RuleTwo r_two/*, sth*/){
	
}