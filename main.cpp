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
#include <algorithm>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
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
    
    /* Another Noob randomize
    random_shuffle(ret.begin(), ret.end());
    */
    
    ret = baseStr;
    shuffle(ret, g.NumCities);
    return ret;
}
vector<Chromosome> makePopulation(Graph &g, int population)
{
    vector<Chromosome> ret;
    unordered_set<string> set;
    for (int i = 0; ret.size() != population; i++)
    {
        string s = getRandomSequence(g);
        if (set.find(s) == set.end())
        {
            Chromosome chromosome;
            chromosome.id = i;
            chromosome.sequence = s;
            chromosome.isFitnessCalculated = false;
            ret.push_back(chromosome);
            set.insert(s);
        }
    }
    cout << "SET SIZE " << set.size() << endl;
    return ret;
}
/*Genetic helpers*/
void calculateFitness(vector<Chromosome> &chromosomes, Graph &g)
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
            sum = ((1.0) / sum);
            totalFitness += sum;
            chromosomes[i].fitVal = sum;
            chromosomes[i].isFitnessCalculated = true;
        }
    }
}
// Could be added to a Chromosome.cpp
pair<Chromosome, Chromosome> gxCrossover(Chromosome p1, Chromosome p2)
{
    Chromosome c1, c2;
    c1.sequence = p1.sequence;
    c2.sequence = p2.sequence;
    c1.isFitnessCalculated = false;
    c2.isFitnessCalculated = false;

    unordered_map<char, char> umap1;
    unordered_map<char, char> umap2;

    for (int i = 0; i < c1.sequence.length() - 1; i++)
    {
        umap1.insert({c1.sequence[i], c1.sequence[i + 1]});
        umap2.insert({c2.sequence[i], c2.sequence[i + 1]});
    }
    umap1.insert({c1.sequence[c1.sequence.length() - 1], c1.sequence[c1.sequence.length() - 2]});
    umap2.insert({c2.sequence[c2.sequence.length() - 1], c2.sequence[c2.sequence.length() - 2]});

    unordered_set<char> alreadyContains;
    alreadyContains.insert(c1.sequence[0]);
    int i;
    for (i = 0; i < c1.sequence.length() - 1; i++)
    {
        char ch1 = umap1[c1.sequence[i]];
        char ch2 = umap2[c1.sequence[i]];
        if (alreadyContains.find(ch1) != alreadyContains.end())
        {
            if (alreadyContains.find(ch2) != alreadyContains.end())
            {
                int rIndex = rand() % gGlobal.NumCities;
                while (alreadyContains.find(p1.sequence[rIndex]) != alreadyContains.end())
                    rIndex = rand() % gGlobal.NumCities;
                c1.sequence[i + 1] = p1.sequence[rIndex];
                alreadyContains.insert(p1.sequence[rIndex]);
            }
            else
            {
                c1.sequence[i + 1] = ch2;
                alreadyContains.insert(ch2);
            }
        }
        else if (alreadyContains.find(ch2) != alreadyContains.end())
        {
            c1.sequence[i + 1] = ch1;
            alreadyContains.insert(ch1);
        }
        else
        {
            int id = getIDFromChar(c1.sequence[i]) - 1;
            int id1 = getIDFromChar(ch1) - 1;
            int id2 = getIDFromChar(ch2) - 1;
            double distance1 = gGlobal.distances[id][id1];
            double distance2 = gGlobal.distances[id][id2];
            if (distance1 > distance2)
            {
                c1.sequence[i + 1] = ch2;
                alreadyContains.insert(ch2);
            }
            else
            {
                c1.sequence[i + 1] = ch1;
                alreadyContains.insert(ch1);
            }
        }
    }
    string tempStr = c1.sequence;
    reverse(tempStr.begin(), tempStr.end());
    c2.sequence = tempStr;
    return make_pair(c1, c2);
}
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
    c1.isFitnessCalculated = false;
    Chromosome c2;
    c2.sequence = p2.sequence;
    c1.isFitnessCalculated = false;

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
    /*Random Parent Selection*/
    srand(time(NULL));
    while (children.size() != numParents)
    {
        int i = rand() % numParents;
        int j = rand() % numParents;
        while (j == i)
            j = rand() % numParents;
        Chromosome p1 = population[i];
        Chromosome p2 = population[j];
        pair<Chromosome, Chromosome> childrenFromPMX = pmxCrossover(p1, p2);
        pair<Chromosome, Chromosome> childrenFromGX = gxCrossover(p1, p2);
        children.push_back(childrenFromPMX.first);
        children.push_back(childrenFromPMX.second);
        children.push_back(childrenFromGX.first);
        children.push_back(childrenFromGX.second);
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
void mutate(vector<Chromosome> &children)
{
    srand(time(NULL));
    for (int index = 0; index < children.size(); index++)
    {
        int count = 0;
        while (count < numCities)
        {
            if (rand() % 100 == 1)
            {
                int i = rand() % numCities;
                int j = rand() % numCities;
                while (j == i)
                    j = rand() % numCities;
                char temp = children[index].sequence[i];
                children[index].sequence[i] = children[index].sequence[j];
                children[index].sequence[j] = temp;
            }
            count++;
        }
    }
}
void mutateAndAddToPopulation(vector<Chromosome> &children, vector<Chromosome> &chromosomes)
{
    mutate(children); //mutation probability per gene is 0.1
    chromosomes = slice(chromosomes, 0, P - 1);
    chromosomes.insert(chromosomes.end(), children.begin(), children.end());
    InitPopulation = chromosomes.size();
}
/*TSP solver*/
Chromosome travellingSalesman(Graph &g)
{
    vector<Chromosome> chromosomes;
    chromosomes = makePopulation(g, InitPopulation);
    srand(time(NULL));
    calculateFitness(chromosomes, g);
    sort(chromosomes.begin(), chromosomes.end(), descSort());
    Chromosome ret = chromosomes[0];
    cout << "At iter 0"
         << " "
         << ": " << ret.dist << endl;
    int i = 0;
    while (i < maxIter)
    {
        //Selection Criteria is currently top P parents in the population
        vector<Chromosome> children = crossover(chromosomes, P); //TODO: introduce crossover probability
        mutateAndAddToPopulation(children, chromosomes);
        cout << "Population: " << chromosomes.size() << endl;
        calculateFitness(chromosomes, g);
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