#include "Population.hpp"
#include <iostream>

Population::Population() {}

vector<vector<int>>& Population::getPopulation() { return (population); }

int Population::bestFitness()
{
    int min = numeric_limits<int>::max(), fitness{ 0 };
    for (vector<int> t : population) {
        fitness = getFitness(t); 
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
        printTour(t,output);
        output<<"\n";
        ++i;
    }
    return (output);
}

void Population::writeBestTour(ofstream& file)
{
    int fitness = bestFitness();
    for (vector<int> t : population) {
        if (getFitness(t) == fitness) {
            printTour(t,file);
            file<<"\n";
        }
    }
}
