#ifndef POPULATION_H
#define POPULATION_H

#include <fstream>
#include <limits>
#include <vector>

#include "Utils.hpp"

using std::numeric_limits;
using std::ofstream;
using std::ostream;
using std::vector;

class Population {

    friend ostream& operator<<(ostream&, Population&);

public:
    Population();

    vector<vector<int>>& getPopulation();

    int bestFitness(unordered_map<int,pair<double,double>>&);

private:
    vector<vector<int>> population;
};

#endif