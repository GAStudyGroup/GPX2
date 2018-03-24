#ifndef GAUTILS_HPP
#define GAUTILS_HPP

#include <iostream>

#include "Population.hpp"
#include "Config.hpp"
#include "GPX2.hpp"
#include "Opt.hpp"
#include "ImportData.hpp"

namespace GAUtils{

    void init(Population &);

    // avalia a população para decidir se as condições de parada foram satisfeitas
    bool stop(Population&);

    // Gera um tour utilizando Nearest Neighbor
    //vector<City> nearestNeighbor(Map &);
    void fillPopulation(Population &, unsigned);

    // Gera uma nova população
    Population generateNewPopulation(Population&);
    Population crossAllxAllwith2opt(Population&);
    Population crossNBestxAllwithReset(Population&);
    Population crossAllxAllwithNBestAndReset(Population&);

    //Log functions
    std::ofstream* initLogFile();
    void printHeader(std::ostream&);
    void printFooter(std::ostream&,Population&,unsigned,unsigned);
}

#endif