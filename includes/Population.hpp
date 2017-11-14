#ifndef POPULATION_H
#define POPULATION_H

#include <fstream>
#include <limits>
#include <vector>

#include "Tour.hpp"

using std::numeric_limits;
using std::ofstream;
using std::ostream;
using std::vector;

class Population {

    friend ostream& operator<<(ostream&, Population&);

public:
    Population();

    vector<Tour>& getPopulation();

    void addNewTour(Tour);

    int bestFitness();

    void writeBestTour(ofstream&);

private:
    vector<Tour> population;
};

#endif