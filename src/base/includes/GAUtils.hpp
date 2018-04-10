#ifndef GAUTILS_HPP
#define GAUTILS_HPP

#include <fstream>
using std::ofstream;

#include <ostream>
using std::ostream;

#include "Population.hpp"

namespace GAUtils{
    auto sortPopulation = [](vector<int> &a, vector<int> &b) { return getFitness(a) < getFitness(b); 
    };

    //Import the map, the tours generated by the LK and fill the population
    void init(Population &);

    //Evaluates the population to decide whether stopping conditions have been met
    bool stop(Population&,ostream &);

    //If a portion of the population has been imported from the KL, the function populates the population up to the value set in the global Config::POP_SIZE using 2-opt. Otherwise, it generates the entire population using 2-op
    void fillPopulation(Population &, unsigned);

    //Generates a new generation obeying the criteria defined in the global Config::TYPE
    Population generateNewPopulation(Population&);
    //Creates a new generation by performing the crossover of everyone with everyone and applying 2-opt in the output
    Population crossAllxAllwith2opt(Population&);
    //Creates a new generation by performing the crossover with the best n and saving them for the next generation, the rest of the population is generated with 2-opt
    Population crossNBestxAllwithReset(Population&);
    //Creates a new generation by performing the crossover of all with all, and passing the best n to the next generation, the rest of the population is generated using the 2-opt
    Population crossAllxAllwithNBestAndReset(Population&);

    //Log functions
    ofstream* initLogFile();
    void printHeader(ostream&);
    void printFooter(ostream&,Population&,unsigned,unsigned);
    void printTime(ostream&,string,double,double);
}

#endif