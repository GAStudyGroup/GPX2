#include "Population.hpp"

Population::Population() {}

vector<vector<int>>& Population::getPopulation() { return (population); }

int Population::bestFitness(unordered_map<int,pair<double,double>> &map)
{
    int min = numeric_limits<int>::max(), fitness{ 0 };
    for (vector<int> t : population) {
        fitness = getFitness(t,map);
        if (min > fitness) {
            min = fitness;
        }
    }
    return (min);
}

ostream& operator<<(ostream& output, Population& pop)
{ // Overload de operador para impressão da população
    int i{ 0 };
    for (vector<int> t : pop.getPopulation()) {
        output << "Tour " << i << "\n";
        for(int id : t){
            output << id << "\n";
        }
        ++i;
    }
    return (output);
}

