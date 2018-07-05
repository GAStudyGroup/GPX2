#if !defined(Constants_HPP)
#define Constants_HPP

#include<vector>
using std::vector;

#include "Ant.hpp"

namespace AntConstants{
    extern double alfa;
    extern double beta;
    extern double Q;
    extern double rho;
    extern double Pbest;
};

namespace AntMap{
    extern double **pheromoneMap;
    extern int bestFitnessYet;
    extern Ant bestAntYet;

    double getPheromoneLevel(int a, int b);

    void setPheromoneLevel(int a, int b, double newValue);

    //true if edge AB is present in ant path
    bool constainsEdge(const vector<int>& path,int a, int b);

    void updatePheromoneMap(const vector<int>& path);

    void printPheromoneMap();

    double maxPheromoneLevel();

    double minPheromoneLevel();
}

#endif // Constants_HPP
