# Solving Travelling Salesman using Genetic Algorithms
## Build status
[![Build Status](https://travis-ci.org/akshittyagi/tsp.svg?branch=master)](https://travis-ci.org/akshittyagi/tsp)
## Running the code
```Makefile
 make
 make run
 ```
Or alternatively use:
```Makefile
 make
 ./omp <InputFileName> <NumThreads>
 ```

## Algorithm
<p>Given the conditions of the problem at hand I have chosen the initial population of the first generation to be 1000 with a P of 256( P is the number of the fittest parents selected to crossover with a probability of 0.8 ).
The algorithm works as follows : it first initialises the population with a set of chromosomes all distinct as can be seen from the use of std::set<T>.
This is sorted and the initial guess is stored. After entering the while loop for iterations I first crossover with PMX and GX and use their offsprings and the parents 
of these offsprings as the new population. This is sorted and the fitness of the new children is evaluated. 
</p>
<p>
For the crossover technique I have used 2 offsprings from PMX and 2 from GX.
I have used a hashing from city ids to characters using a startingChar as offset.
The initialisation algorithm is Fisher Yates for randomly shuffling a string which is able to produce a completely distinct set of strings till upto ~2800 iterations.
</p>
<p>
Parallelisation has been done in crossover and mutation steps as these were the two heavy ops per iteration for the convergence of the GA.
Special care had to be taken in the case of different seeds for rand() for different threads which has been taken care of.
[ calculateFitness() 's parllelisation didnt yield good results]
The for loops have been parallelised using standard omp technqiues and use the static load balancing.
</p>


## Performance
For a test case of 32 cities following are the running times: (4 core, i7-1.8Ghz)

| Threads       | Running Times(s) |
|:-------------:|:----------------:|
| 4             | 15.6586          |
| 6             | 5.814            |
| 8             | 4.824            |
| 12            | 3.489            |
| 16            | 3.005            |
| 24            | 2.744            |
| 32            | 2.610            |
