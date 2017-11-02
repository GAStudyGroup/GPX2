#ifndef TOUR_HPP
#define TOUR_HPP

#include "City.hpp"
#include <cmath>
#include <vector>

using std::vector;

class Tour {
public:
    Tour();
    Tour(vector<City>);

    void setRoute(vector<City>);

    vector<City>& getRoute();

    double getFitness();

    double distance(const int,const int);

private:
    vector<City> route;
};

#endif