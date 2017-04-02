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
    string line;
    fstream file;
    file.open(filename, fstream::in);
    int i = 0;
    while (getline(file, line))
    {
        if (line == "EOF" or line == " EOF" or line == "EOF " or line == " EOF ")
            continue;
        i++;
        if (i >= 4)
        {
            int num1, id1, id2;
            vector<string> split = splitStr(line, ' ');
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

            float num2;
            if (split[1][0] == '-')
            {
                if (split[1].length() == 6)
                    num2 = -1 * (10 * (split[1][1] - '0') + (split[1][2] - '0') + (split[1][4] - '0') * (0.1) + (split[1][5] - '0') * (0.01));

                if (split[1].length() == 5)
                    num2 = -1 * ((split[1][1] - '0') + (split[1][3] - '0') * (0.1) + (split[1][4] - '0') * (0.01));
            }
            else
            {
                if (split[1].length() == 5)
                    num2 = (10 * (split[1][0] - '0') + (split[1][1] - '0') + (split[1][3] - '0') * (0.1) + (split[1][4] - '0') * (0.01));

                if (split[1].length() == 4)
                    num2 = ((split[1][0] - '0') + (split[1][2] - '0') * (0.1) + (split[1][3] - '0') * (0.01));
            }

            float num3;
            if (split[2][0] == '-')
            {
                if (split[2].length() == 6)
                    num3 = -1 * (10 * (split[2][1] - '0') + (split[2][2] - '0') + (split[2][4] - '0') * (0.1) + (split[2][5] - '0') * (0.01));

                if (split[2].length() == 5)
                    num3 = -1 * ((split[2][1] - '0') + (split[2][3] - '0') * (0.1) + (split[2][4] - '0') * (0.01));
            }
            else
            {
                if (split[2].length() == 5)
                    num3 = (10 * (split[2][0] - '0') + (split[2][1] - '0') + (split[2][3] - '0') * (0.1) + (split[2][4] - '0') * (0.01));

                if (split[2].length() == 4)
                    num3 = ((split[2][0] - '0') + (split[2][2] - '0') * (0.1) + (split[2][3] - '0') * (0.01));
            }

            pair<double, double> p1(num2, num3);
            pair< int, pair<double, double> > p(num1, p1);
            cities.push_back(p);
        }
    }
    NumCities = i - 3;
    file.close();
    makeAdjMatrix();
}
// int main()
// {
//     Graph g;
//     g.makeGraph("sample_input.txt");
//     return 0;
// }