# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

gccopt= gcc -O2 -funroll-loops
gccdeb= gcc -ggdb -Wall -pedantic
gppdeb = g++ $(CCDEB) 
#CCC = $(gccopt)
CCC = $(gccdeb)

CFLAGS=
LINUXLIBS= -lm -lpthread

LIBS= $(LINUXLIBS) 


#---------------- GUROBI -----------------------------------
furrybabyGRB = /opt/gurobi600/linux64

GRB = $(furrybabyGRB)
GRBLNFLAGS = -L$(GRB)/lib -lgurobi60

#GRB = $(fatpuppyGRB)

GRBLPINCDIR = $(GRB)/include

#------------ LPSOLVER CHOICE MADE HERE ---------------

CCCLNFLAGS = $(GRBLNFLAGS) $(LIBS)
LPINCDIR = $(GRBLPINCDIR)

CCCFLAGS = -I$(LPINCDIR) 

../bin/%.o: %.c
	@echo compiling $*.c with $(CCC) $(CCCFLAGS)
	@$(CCC) $(CCCFLAGS) -c $< -o $@

RULEGRB = ../bin/main.o ../bin/decomposer.o ../bin/power.o ../bin/powerbag.o ../bin/rulebag.o ../bin/inputreader.o ../bin/worker.o

rule: $(RULEGRB)
	$(CCC) $(CCCFLAGS) -o ../bin/main $(RULEGRB) $(CCCLNFLAGS)


clean:
	rm ../bin/*

seep:
	rm *~
	rm *#
