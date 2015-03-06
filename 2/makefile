CC = gcc
CFLAGS = -Wall -pedantic
LIBS = -lm

SRC = src
DAT = data
TGT1 = master
TGT2 = nerd
TGT3 = nerdprocess
UTIL = utilities
EXECM = e_master
EXECN = e_nerd

all: clean $(TGT1) $(TGT2)

$(TGT1): clean1
	$(CC) $(CFLAGS) ./$(SRC)/$(TGT1).c ./$(SRC)/$(UTIL).c $(LIBS) -o $(TGT1)

$(TGT2): clean2
	$(CC) $(CFLAGS) ./$(SRC)/$(TGT2).c ./$(SRC)/$(TGT3).c ./$(SRC)/$(UTIL).c $(LIBS) -o $(TGT2)

$(EXECM):
	./$(TGT1) ./$(DAT)/hw2_list.dat ./$(DAT)/hw2_const.dat $(N)

$(EXECN):
	./$(TGT2) $(N)

clean: clean1 clean2

clean1:
	$(RM) $(TGT1)
	
clean2:
	$(RM) $(TGT2)
