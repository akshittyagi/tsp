#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "Graph.h"

using namespace std;
vector<string> splitStr(string line, char ch)
{
    string formed = "";
    vector<string> ret;
    for (int i = 0; i < line.length(); i++)
    {
        if (line[i] == ch)
        {
            if (i != 0)
                ret.push_back(formed);
            formed = "";
            continue;
        }
        else
            formed += line[i];
    }
    if (formed != "")
        ret.push_back(formed);

    return ret;
}

double euclideanDistance(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void Graph::makeAdjMatrix()
{
    for (int i = 0; i < cities.size(); i++)
    {
        vector<double> ret;
        for (int j = 0; j < cities.size(); j++)
            ret.push_back(euclideanDistance(cities[i].second.first, cities[i].second.second, cities[j].second.first, cities[j].second.second));
        distances.push_back(ret);
    }
    return;
}
void Graph::makeGraph(const char *filename)
{
    string line, nameStr;
    double x, y; // x and y coordinates
    ifstream file("sample_input.txt");
    int n;
    file >> nameStr >> nameStr >> line >> n >> line;
    string id;
    for (int i = 0; i < n; i++)
    {
        file >> id >> x >> y;
        int num1, id1, id2;
        vector<string> split = splitStr(id, ' ');
        if (split[0].length() == 2)
        {
            id1 = split[0][0] - '0';
            id2 = split[0][1] - '0';
            num1 = id1 * 10 + id2;
        }
        else if (split[0].length() == 1)
        {
            id1 = split[0][0] - '0';
            num1 = id1;
        }
        pair< double, double > p1(x, y);
        pair< int, pair< double, double > > p(num1, p1);
        cities.push_back(p);
    }

    NumCities = n;
    file.close();
    makeAdjMatrix();
}
// int main()
// {
//     Graph g;
//     g.makeGraph("sample_input.txt");
//     return 0;
// }