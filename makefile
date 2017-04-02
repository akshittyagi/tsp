CC = g++
CFLAGS = -fopenmp -O3 -g
OBJS = main.o Graph.o
EXECNAME = omp
TESTNAME = Graph

all: parallel

parallel: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXECNAME)

main.o: main.cpp Graph.o Chromosome.h
	$(CC) $(CFLAGS) -c main.cpp

Graph.o:
	$(CC) $(CFLAGS) -c Graph.cpp
	
run: parallel
	./$(EXECNAME)

clean:
	rm -f *~ *.o parallel
	rm $(EXECNAME)
	rm $(TESTNAME)
