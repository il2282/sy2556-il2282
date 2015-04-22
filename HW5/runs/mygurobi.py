#!/usr/bin/python

import sys
import math
from gurobipy import *

if len(sys.argv) < 2:
    print('Usage: myguropi.py lpfilename')
    exit(0)

log = open("mygurobi.log","w")

# Read and solve model

model = read(sys.argv[1])
model.optimize()

log.write('Solving %s\n' % sys.argv[1])
log.write("variables = " + str(model.NumVars) + "\n")
log.write("constraints = " + str(model.NumConstrs) + "\n")

if model.status == GRB.status.INF_OR_UNBD:
    print "->LP infeasible or unbounded"
    log.write('->LP infeasible or unbounded\n')
    log.close()
    exit(0)

log.write('Optimal objective = %g\n' % model.objVal)

count = 0
for v in model.getVars():
    if math.fabs(v.x) > 0.0000001:
        count += 1

log.write(str(count) + " nonzero variables in solution\n")
for v in model.getVars():
    if math.fabs(v.x) > 0.0000001:
        print( v.varname + " = " +str(v.x))
        log.write( v.varname + " = " +str(v.x) + "\n")

log.write("bye.\n")
log.close()
