/**
* Underlying assumptions:
* Graph Node Ids are integers given in ascending order
*/
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <algorithm> //Only for std::sort
#include <vector>
#include <iostream>
#include <unordered_map>
#include "Graph.h"
#include "Chromosome.h"

#define NumThreads 8
#define startingChar 48
#define P 256
#define maxIter 10000
using namespace std;

int totalFitness;
vector<Chromosome> updatedPopulation;
int numCities;
string baseStr;
Graph gGlobal;
int InitPopulation = 2800;
/*Hashing Function*/
int getIDFromChar(char ch)
{
    return (ch - startingChar);
}
/*Comparator*/
struct descSort
{
    inline bool operator()(const Chromosome &chrome1, const Chromosome &chrome2)
    {
        return (chrome1.fitVal > chrome2.fitVal);
    }
};
/*Fisher Yates*/
int rand_int(int n)
{
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;

    do
    {
        rnd = rand();
    } while (rnd >= limit);
    return rnd % n;
}

void shuffle(string &str, int n)
{
    int i, j;
    char tmp;

    for (i = n - 1; i > 0; i--)
    {
        j = rand_int(i + 1);
        tmp = str[j];
        str[j] = str[i];
        str[i] = tmp;
    }
}

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

    /* Noob randomize

    int start = rand() % g.NumCities;
    //printf("Start: %d\n",start);
    int count = 0;
    for (int j = start; count < g.NumCities; j = (j + 1) % g.NumCities)
    {
        char ch = g.cities[j].first + startingChar;
        ret += ch;
        count++;
    }
    */

    ret = baseStr;
    //random_shuffle(ret.begin(), ret.end());
    shuffle(ret,g.NumCities);
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
    for (int i = 0; i < chromosomes.size(); i++)
    {
        if (!chromosomes[i].isFitnessCalculated)
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
            chromosomes[i].dist = sum;
            //sum = sum / ScalingFactor;
            sum = ((1.0) / sum);
            array[i] = sum;
            totalFitness += array[i];
            chromosomes[i].fitVal = array[i];
            chromosomes[i].isFitnessCalculated = true;
        }
    }
}
// Could be added to a Chromosome.cpp
// pair<Chromosome, Chromosome> gxCrossover(Chromosome p1, Chromosome p2)
// {
//     Chromsome c1, c2;
//     c1.sequence = p1.sequence;
//     c2.sequence = p2.sequence;

//     bool hasBeenSelected[gGlobal.NumCities + 1];
//     fill_n(hasBeenSelected, numCities, false);

//     for (int i = 0; i < c1.sequence.length(); i++)
//     {
//         if (i < c1.sequence.length() - 1)
//         {
//             char ch = c1.sequence[i];
//             double distance1 = gGlobal.distances[getIDFromChar(ch)][getIDFromChar(c1.sequence[i + 1])];
//             int found = -1;
//             for (int j = 0; j < c2.sequence.length(); j++)
//                 if (ch == c2.sequence[j])
//                     found = j;
//             double distance2;
//             int oldFound = found;
//             if (found < c2.sequence.length() - 1)
//             {
//                 distance2 = gGlobal.distances[getIDFromChar(ch)][getIDFromChar(c2.sequence[found + 1])];
//                 found = found + 1;
//             }
//             else
//             {
//                 distance2 = gGlobal.distances[getIDFromChar(ch)][getIDFromChar(c2.sequence[found - 1])];
//                 found = found - 1;
//             }

//             if (distance1 < distance2)
//             {
//                 if (!hasBeenSelected[getIDFromChar(c2.sequence[found]) + 1])
//                 {
//                     c1.sequence[i + 1] = c2.sequence[found];
//                     hasBeenSelected[getIDFromChar(c2.sequence[found]) + 1] = true;
//                 }
//                 else
//                 {
//                 }
//             }
//         }
//     }
// }
pair<Chromosome, Chromosome> pmxCrossover(Chromosome p1, Chromosome p2)
{
    int i = rand() % (p1.sequence.length() / 2);
    int j = i + rand() % (p1.sequence.length() / 2) + 1;
    int start = i;
    int end = j;
    string mapStr1 = p1.sequence.substr(i, j - i + 1);
    string mapStr2 = p2.sequence.substr(i, j - i + 1);
    unordered_map<char, char> umap1;
    unordered_map<char, char> umap2;
    Chromosome c1;
    c1.sequence = p1.sequence;
    Chromosome c2;
    c2.sequence = p2.sequence;

    int count = 0;
    for (i = start; i <= end; i++)
    {
        c1.sequence[i] = mapStr2[count];
        c2.sequence[i] = mapStr1[count];
        umap1.insert({mapStr1[count], mapStr2[count]});
        umap2.insert({mapStr2[count], mapStr1[count]});
        count++;
    }

    for (i = 0; i < c2.sequence.length(); i++)
    {
        if (i >= start and i <= end)
            continue;
        char ch = c2.sequence[i];
        while (umap1.find(ch) != umap1.end())
        {
            ch = umap1[ch];
        }
        c2.sequence[i] = ch;
    }
    for (i = 0; i < c1.sequence.length(); i++)
    {
        if (i >= start and i <= end)
            continue;
        char ch = c1.sequence[i];
        while (umap2.find(ch) != umap2.end())
        {
            ch = umap2[ch];
        }
        c1.sequence[i] = ch;
    }

    return make_pair(c1, c2);
}
vector<Chromosome> crossover(vector<Chromosome> &population, int numParents)
{
    vector<Chromosome> children;
    for (int i = 0; i < numParents - 1; i += 2)
    {
        Chromosome p1 = population[i];
        Chromosome p2 = population[i + 1];
        pair<Chromosome, Chromosome> childrenFromPMX = pmxCrossover(p1, p2);
        //pair<Chromosome, Chromsome> childsFromGX = gxCrossover(p1, p2);
        children.push_back(childrenFromPMX.first);
        children.push_back(childrenFromPMX.second);
        // children.push_back(childrenFromGX.first);
        // children.push_back(childrenFromGX.second);
    }
    return children;
}
vector<Chromosome> slice(vector<Chromosome> &v, int start, int end)
{
    int newlen = end - start + 1;
    vector<Chromosome> nv(newlen);

    for (int i = 0; i < newlen; i++)
    {
        nv[i] = v[start + i];
    }
    return nv;
}
void mutateAndAddToPopulation(vector<Chromosome> &children, vector<Chromosome> &chromosomes)
{
    chromosomes = slice(chromosomes, 0, P - 1);
    chromosomes.insert(chromosomes.end(), children.begin(), children.end());
    InitPopulation = chromosomes.size();
}
/*TSP solver*/
Chromosome travellingSalesman(Graph &g)
{
    vector<Chromosome> chromosomes;
    chromosomes = makePopulation(g, InitPopulation);
    double fitnessValues[InitPopulation];
    srand(time(NULL));
    calculateFitness(fitnessValues, chromosomes, g);
    sort(chromosomes.begin(), chromosomes.end(), descSort());
    Chromosome ret = chromosomes[0];
    int i = 0;
    while (i < maxIter)
    {
        //Selection Criteria is currently top P parents in the population
        vector<Chromosome> children = crossover(chromosomes, P);
        mutateAndAddToPopulation(children, chromosomes);
        cout << "Population: " << chromosomes.size() << endl;
        calculateFitness(fitnessValues, chromosomes, g);

        sort(chromosomes.begin(), chromosomes.end(), descSort());

        ret = chromosomes[0];
        i++;
        cout << "At iter " << i << " "
             << ": " << ret.dist << endl;
    }
    return ret;
}

int main()
{
    srand(time(NULL));
    /*Graph Input*/
    Graph g;
    g.makeGraph("sample_input.txt");

    numCities = g.NumCities;
    gGlobal = g;
    baseStr = "";
    for (int i = 1; i <= numCities; i++)
    {
        baseStr += (i + '0');
    }
    Chromosome result = travellingSalesman(g);
    for (int i = 0; i < result.sequence.length(); i++)
        cout << (int)result.sequence[i] - startingChar << " ";
    cout << "\nDistance: " << result.dist << endl;
    return 0;
}