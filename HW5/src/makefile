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

FACTORQPMIP = ../bin/main_f_mip.o ../bin/enginemip.o

factorqpmip: $(FACTORQPMIP)
	$(CCC) $(CCCFLAGS) -o ../bin/factorqpmip $(FACTORQPMIP) $(CCCLNFLAGS)

GENMIP = ../bin/genmain.o ../bin/genengine.o

genmip: $(GENMIP)
	$(CCC) $(CCCFLAGS) -o ../bin/genmip $(GENMIP) $(CCCLNFLAGS)


clean:
	rm ../bin/*

seep:
	rm *~
	rm *#
