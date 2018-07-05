#ifndef ANTCOLONY_HPP
#define ANTCOLONY_HPP

#include "Population.hpp"

#include "Ant.hpp"

class AntColony{
    private:
        int iteration=0;
        void printColony(vector<Ant> colony);
    public:
        AntColony();
        ~AntColony();

        Population run();
};

#endif
