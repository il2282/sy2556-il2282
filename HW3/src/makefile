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
kikiGRB = /home/dano/thirdparty/gurobi/gurobi201/linux32
furrybabyGRB = /opt/gurobi/gurobi600/linux64
rockwaldGRB = /opt/gurobi/gurobi451/linux64
fatpuppyGRB = /opt/gurobi/gurobi563/linux64

GRB = $(furrybabyGRB)
GRBLNFLAGS = -L$(GRB)/lib -lgurobi60

#GRB = $(fatpuppyGRB)

GRBLPINCDIR = $(GRB)/include

#------------ LPSOLVER CHOICE MADE HERE ---------------

#CCCLNFLAGS = $(CPXLNFLAGS) $(LIBS)
#CCCLNFLAGS = $(GRBLNFLAGS) $(LIBS)
#LPINCDIR = $(GRBLPINCDIR) #none for this project

CCCLNFLAGS = $(LIBS)
LPINCDIR =

CCCFLAGS = 

../bin/%.o: %.c
	@echo compiling $*.c with $(CCC) $(CCCFLAGS)
	@$(CCC) $(CCCFLAGS) -c $< -o $@

MAIN = ../bin/main.o ../bin/meanvar.o ../bin/power.o ../bin/utilities.o

main: $(MAIN)
	$(CCC) $(CCCFLAGS) -o ../bin/main $(MAIN) $(CCCLNFLAGS)

clean:
	rm ../bin/*

seep:
	rm *~
	rm *#
