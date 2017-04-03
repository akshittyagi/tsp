/**
* Underlying assumptions:
* Graph Node Ids are integers given in ascending order
*/
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <algorithm> //Only for std::sort
#include <vector>
#include <iostream>
#include "Graph.h"
#include "Chromosome.h"

#define NumThreads 8
#define InitPopulation 500
#define ScalingFactor 10000 //Scaling for fitness eval
#define startingChar 48
#define RandFactor 5
#define P 24
#define maxIter 100

using namespace std;

int totalFitness;
vector<Chromosome> updatedPopulation;
/*Hashing Function*/
int getIDFromChar(char ch)
{
    return (ch - startingChar);
}
/*Comparator*/
struct descSort
{
    inline bool operator() (const Chromosome& chrome1, const Chromosome& chrome2)
    {
        return (chrome1.fitVal > chrome2.fitVal);
    }
};
/* Debugging functions*/
void printCities(Graph &g)
{
    for (int i = 0; i < g.NumCities; i++)
    {
        printf("CityIds: %d Lat:%f Long:%f\n", g.cities[i].first, g.cities[i].second.first, g.cities[i].second.second);
    }
}
void printDistances(Graph &g)
{
    printf("Adjacency Matrix\n");
    for (int i = 0; i < g.NumCities; i++)
    {
        printf("Row[%d]: ", i);
        for (int j = 0; j < g.NumCities; j++)
            printf("%f ", g.distances[i][j]);
        printf("\n");
    }
}
void printArray(void *start, int length, int args)
{
    switch (args)
    {
    //C98 Grammar Quirk
    case -1:;
        double *ptr0;
        ptr0 = (double *)start;
        for (int i = 0; i < length; i++)
            printf("%f\n", *(ptr0 + i));
        printf("\n");
        break;
    //C98 Grammar Quirk
    case 0:;
        char *ptr1;
        ptr1 = (char *)start;
        for (int i = 0; i < length; i++)
            printf("%c", *(ptr1 + i));
        printf("\n");
        break;
    //C98 Grammar Quirk
    case 1:;
        int *ptr2;
        ptr2 = (int *)start;
        for (int i = 0; i < length; i++)
            printf("%d\n", *(ptr2 + i));
        printf("\n");
        break;
    default:;
        char **ptr3;
        ptr3 = (char **)start;
        for (int i = 0; i < length; i++)
        {
            printf("Chromosomes[%d]: ", i);
            for (int j = 0; j < args; j++)
                printf("%c", ptr3[i][j]);
            printf("\n");
        }
    }
}
/*Helper functions*/
string getRandomSequence(Graph &g)
{
    string ret = "";
    int start = rand() % g.NumCities;
    //printf("Start: %d\n",start);
    int count = 0;
    for (int j = start; count < g.NumCities; j = (j + 1) % g.NumCities)
    {
        char ch = g.cities[j].first + startingChar;
        ret += ch;
        count++;
    }
    return ret;
}
vector<Chromosome> makePopulation(Graph &g, int population)
{
    vector<Chromosome> ret;
    for (int i = 0; i < population; i++)
    {
        string s = getRandomSequence(g);
        Chromosome chromosome;
        chromosome.id = i;
        chromosome.sequence = s;
        ret.push_back(chromosome);
    }
    return ret;
}
/*Genetic helpers*/
void calculateFitness(double array[], vector<Chromosome> &chromosomes, Graph &g)
{
    totalFitness = 0;
    for (int i = 0; i < InitPopulation; i++)
    {
        double sum = 0;
        string s = chromosomes[i].sequence;
        int id1, id2;
        for (int j = 0; j < g.NumCities - 1; j++)
        {
            id1 = getIDFromChar(s[j]);
            id2 = getIDFromChar(s[j + 1]);
            sum = sum + g.distances[id1 - 1][id2 - 1];
        }
        sum = sum / ScalingFactor;
        sum = ((1.0) / sum);
        array[i] = sum + rand() % RandFactor;
        totalFitness += array[i];
        chromosomes[i].fitVal = array[i];
    }
}
vector<Chromosome> crossover(vector<Chromosome> population, int numParents)
{
    vector<Chromsome> children;
    for(int i=0;i<numParents-1;i+=2)
    {
        //do PMX, GX

        //add to set of children
    }
    return children;

}
/*TSP solver*/
Chromosome travellingSalesman(Graph &g)
{
    vector<Chromosome> chromosomes;
    chromosomes = makePopulation(g, InitPopulation);
    double fitnessValues[InitPopulation];
    srand(time(NULL));
    calculateFitness(fitnessValues, chromosomes, g);
    sort(chromosomes.begin(),chromosomes.end(),descSort());
    Chromosome ret = chromosomes[0];
    int i = 0;
    while(i < maxIter)
    {
        //Selection Criteria is currently top P parents in the population
        vector<Chromosome> children = crossover(chromosomes,P);
        mutateAndAddToPopulation(children, chromosomes);
        calculateFitness(fitnessValues, chromosomes, g);
        sort(chromosomes.begin(),chromosomes.end(),descSort());
        ret = chromosomes[0];    
    }
}

int main()
{
    srand(time(NULL));
    /*Graph Input*/
    Graph g;
    g.makeGraph("sample_input.txt");
    Chromsome result = travellingSalesman(g);
    return 0;
}