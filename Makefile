CSPATH = CutSplit/
CTPATH = CutTSS/
OVSPATH = OVS/
PSPATH = PartitionSort/
HSPATH = HyperSplit/
NCPATH = NeuroCuts/
VPATH = $(CSPATH) $(CTPATH) $(OVSPATH) $(PSPATH) $(HSPATH) $(NCPATH)

CPP=g++
JSON_INCLUDE_PATH = /opt/homebrew/Cellar/nlohmann-json/3.11.2/include/
INCLUDE = -I$(JSON_INCLUDE_PATH)
CFLAGS = -g -w -std=c++14 -fpermissive -O3 $(INCLUDE)

# Targets needed to bring the executable up to date

main: main.o CutSplit.o
	$(CPP) $(CFLAGS) -o main *.o $(LIBS)
# ---------------------------------------------------------------------------------------------------------



HyperSplit.o: HyperSplit.h HyperSplit.cpp ElementaryClasses.h
	$(CPP) $(CFLAGS) -c $(HSPATH)HyperSplit.cpp

CutSplit.o: CutSplit.h CutSplit.cpp ElementaryClasses.h HyperSplit.h HyperSplit.cpp
	$(CPP) $(CFLAGS) -c $(CSPATH)CutSplit.cpp

CutTSS.o: CutTSS.cpp CutTSS.h TupleSpaceSearch.h TupleSpaceSearch.cpp ElementaryClasses.h
	$(CPP) $(CFLAGS) -c $(CTPATH)CutTSS.cpp

# ** TupleSpace **
OVS.o: $(wildcard $(OVSPATH)*.cpp) ElementaryClasses.h
	$(CPP) $(CFLAGS) -c $(OVSPATH)*.cpp

# ** PartitionSort **
PartitionSort.o: $(wildcard $(PSPATH)*.cpp) ElementaryClasses.h
	$(CPP) $(CFLAGS) -c $(PSPATH)*.cpp

NeuroCuts.o: $(wildcard $(NCPATH)*.cpp) ElementaryClasses.h
	$(CPP) $(CFLAGS) -c $(NCPATH)*.cpp

main.o: main.cpp CutTSS.h CutSplit.h TupleSpaceSearch.h ElementaryClasses.h
	$(CPP) $(CFLAGS) -c main.cpp


all:main
clean:
	rm -rf *.o main
