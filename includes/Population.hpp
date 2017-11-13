#ifndef POPULATION_H
#define POPULATION_H

#include "Tour.hpp"
#include <iostream>
#include <limits>
#include <vector>
#include <fstream>

using std::cout;
using std::endl;
using std::numeric_limits;
using std::ostream;
using std::setfill;
using std::setw;
using std::vector;
using std::ofstream;

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