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

$(TGT1):
	$(CC) $(CFLAGS) ./$(SRC)/$(TGT1).c ./$(SRC)/$(UTIL).c $(LIBS) -o $(TGT1)

$(TGT2): 
	$(CC) $(CFLAGS) ./$(SRC)/$(TGT2).c ./$(SRC)/$(TGT3).c ./$(SRC)/$(UTIL).c $(LIBS) -o $(TGT2)

$(EXECM):
	./$(TGT1) ./$(DAT)/hw2_list.dat ./$(DAT)/hw2_const.dat $(N)

$(EXECN):
	./$(TGT2) $(N)

clean:
	$(RM) $(TGT1)
	$(RM) $(TGT2)