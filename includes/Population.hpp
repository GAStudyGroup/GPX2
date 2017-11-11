#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <iostream>
#include <limits>
#include "Tour.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::ostream;
using std::setfill;
using std::setw;
using std::numeric_limits;

class Population{

   friend ostream& operator<<(ostream&,Population &);
   
public:
   Population();   

   vector<Tour>& getPopulation();

   void addNewTour(Tour); 

   int bestFitness();
private:
   vector<Tour> population;
};

#endif