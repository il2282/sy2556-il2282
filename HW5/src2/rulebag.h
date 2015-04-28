#ifndef RULEBAG

#define RULEBAG

#define NOMEMORY 10

#define LONG  1
#define SHORT  -1

typedef struct RuleOne{
	double a, b;
	int N_one, N_two;
}RuleOne;

typedef struct RuleTwo{
	int numindices;
	int *indices;
	double delta;
	int direction;
}RuleTwo;

int allocateruletwoindices(RuleTwo *p_rule);
void freeruletwoindices(RuleTwo *p_rule);

void makegrbruleone(RuleOne r_one/*, sth*/);
void makegrbruletwo(RuleTwo r_two/*, sth*/);

#endif