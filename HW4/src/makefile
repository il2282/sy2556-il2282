# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

gccopt= gcc -O2 -funroll-loops
gccdeb= gcc -ggdb -Wall -pedantic
gppdeb = g++ $(CCDEB) 
#CCC = $(gccopt)
CCC = $(gccdeb)

#CFLAGS=
LINUXLIBS= -lm -lpthread

LIBS= $(LINUXLIBS) 


#---------------- GUROBI -----------------------------------
furrybabyGRB = /opt/gurobi600/linux64

GRB = $(furrybabyGRB)
GRBLNFLAGS = -L$(GRB)/lib -lgurobi60

#GRB = $(fatpuppyGRB)

GRBLPINCDIR = $(GRB)/include


#------------ LPSOLVER CHOICE MADE HERE ---------------

#CCCLNFLAGS = $(CPXLNFLAGS) $(LIBS)
CCCLNFLAGS = $(GRBLNFLAGS) $(LIBS)
LPINCDIR = $(GRBLPINCDIR) #none for this project

CCCFLAGS = -I$(LPINCDIR)

../bin/%.o: %.c
	@echo compiling $*.c with $(CCC) $(CCCFLAGS)
	@$(CCC) $(CCCFLAGS) -c $< -o $@

MAIN = ../bin/engine.o ../bin/main.o ../bin/power.o ../bin/utilities.o ../bin/powerbag.o ../bin/seriesoperation.o  ../bin/worker.o


all: clean power

power: $(MAIN)
	$(CCC) $(CCCFLAGS) -o ../bin/main $(MAIN) $(CCCLNFLAGS)

clean:
	rm ../bin/*

seep:
	rm *~
	rm *#

