#include <utility>
#include <string>
#include <vector>

class Graph
{
public:
  int NumCities;
  Graph()
  {
    NumCities = 0;
  }
  std::vector< std::pair<int, std::pair<double, double> > > cities;
  std::vector< std::vector<double> > distances;
  void makeGraph(const char *filename);
  void makeAdjMatrix();
};