#ifndef ANTCOLONY_HPP
#define ANTCOLONY_HPP

#include "Population.hpp"

#include "Ant.hpp"

class AntColony{
    private:
        static int iteration;
        void printColony(vector<Ant>);
    public:
        Population run(unsigned);
};

#endif
