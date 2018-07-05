#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
using std::vector;

#include <ostream>
using std::ostream;

#include "Utils.hpp"

class Population {
    friend ostream& operator<<(ostream&, Population&);
public:
    Population();

    vector<vector<int>>& getPopulation();

    vector<int>& getBestTour();

    int bestFitness();
private:
    vector<vector<int>> population;
};

#endif