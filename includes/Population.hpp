#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <iostream>
#include "Tour.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::ostream;
using std::setfill;
using std::setw;

class Population{

   friend ostream& operator<<(ostream&,Population &);
   
public:
   Population();   

   vector<Tour>& getPopulation();

   void addNewTour(Tour&); 
private:
   vector<Tour> population;
};

#endif