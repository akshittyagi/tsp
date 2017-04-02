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

using namespace std;

int hashArray[256];
int totalFitness;
vector<Chromosome> updatedPopulation;
/*Hashing Function*/
int getIDFromChar(char ch)
{
    return hashArray[(int)ch];
}
/*Comparator*/
bool comparator(double i,double j)
{
    return i>j;
}
/* Debugging functions*/
void printCities(Graph &g)
{
    for (int i = 0; i < g.NumCities; i++)
    {
        printf("CityIds: %d Lat:%f Long:%f\n", g.cities[i].first,g.cities[i].second.first,g.cities[i].second.second);
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
// void printArray(void *start, int length, int args)
// {
//     switch (args)
//     {
//     case -1:;
//         double *ptr0;
//         ptr0 = (double *)start;
//         for (int i = 0; i < length; i++)
//             printf("%f\n", *(ptr0 + i));
//         printf("\n");
//         break;
//     case 0:;
//         char *ptr1;
//         ptr1 = (char *)start;
//         for (int i = 0; i < length; i++)
//             printf("%c", *(ptr1 + i));
//         printf("\n");
//         break;
//     case 1:;
//         int *ptr2;
//         ptr2 = (int *)start;
//         for (int i = 0; i < length; i++)
//             printf("%d\n", *(ptr2 + i));
//         printf("\n");
//         break;
//     default:;
//         char **ptr3;
//         ptr3 = (char **)start;
//         for (int i = 0; i < length; i++)
//         {
//             printf("Chromosomes[%d]: ", i);
//             for (int j = 0; j < args; j++)
//                 printf("%c", ptr3[i][j]);
//             printf("\n");
//         }
//     }
// }
// /*Helper functions*/
// char *getRandomSequence(Graph *g)
// {
//     char *ret;
//     ret = (char *)malloc(g->NumCities * sizeof(char *));
//     int start = rand() % g->NumCities;
//     //printf("Start: %d\n",start);
//     int count = 0;
//     for (int j = start; count < g->NumCities; j = (j + 1) % g->NumCities)
//     {
//         char ch = g->cities[j];
//         *(ret + count) = ch;
//         count++;
//     }
//     return ret;
// }
// char **makePopulation(Graph *g, int population)
// {
//     char **ret;
//     ret = (char **)malloc(population * sizeof(char *));
//     for (int i = 0; i < population; i++)
//     {
//         ret[i] = getRandomSequence(g);
//     }

//     return ret;
// }
// /*Genetic helpers*/
// void calculateFitness(double array[], char **chromosomes, Graph *g)
// {
//     totalFitness = 0;
//     for (int i = 0; i < InitPopulation; i++)
//     {
//         double sum = 0;
//         char *curr = chromosomes[i];
//         for (int j = 0; j < g->NumCities - 1; j++)
//         {
//             int id1 = getIDFromChar(curr[j]);
//             int id2 = getIDFromChar(curr[j + 1]);
//             sum = sum + g->distances[id1][id2];
//         }
//         sum = sum / ScalingFactor ;
//         int rando = rand()%ScalingFactor;
//         sum = ((1.0) / sum) + (rando)/(ScalingFactor);
//         array[i] = sum;
//         totalFitness += array[i];
//     }
// }
/*TSP solver*/
// void travellingSalesman(Graph &g)
// {
//     vector<Chromosomes> chromosomes;
//     chromosomes = makePopulation(g, InitPopulation);

//     double fitnessValues[InitPopulation];
//     srand(time(NULL));
//     calculateFitness(fitnessValues, chromosomes, g);

// }

int main()
{
    srand(time(NULL));
    memset(hashArray, 0, 256);
    /*Graph Input*/
    Graph g;
    g.makeGraph("sample_input.txt");
    travellingSalesman(g);
    return 0;
}