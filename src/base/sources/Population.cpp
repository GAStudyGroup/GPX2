#include "Population.hpp"
#include <iostream>

#include <limits>
using std::numeric_limits;

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

vector<int>& Population::getBestTour(){
    int min = numeric_limits<int>::max(), fitness{ 0 },index{-1};
    for(unsigned i=0;i<population.size();i++){
        fitness = getFitness(population[i]);
        if(min>fitness){
            min = fitness;
            index = i;
        }
    }
    return (population[index]);
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
