CC = g++
CFLAGS = -std=c++11 -fopenmp -O3 -g
OBJS = main.o Graph.o
EXECNAME = omp
TESTNAME = Graph
FILENAME = sample_input.txt
NUMTHREADS = 8

all: parallel

parallel: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXECNAME)

main.o: main.cpp Graph.o Chromosome.h
	$(CC) $(CFLAGS) -c main.cpp

Graph.o:
	$(CC) $(CFLAGS) -c Graph.cpp
	
run: parallel
	time ./$(EXECNAME) $(FILENAME) $(NUMTHREADS) 

clean:
	rm -f *~ *.o parallel
	rm $(EXECNAME)
	rm $(TESTNAME)
