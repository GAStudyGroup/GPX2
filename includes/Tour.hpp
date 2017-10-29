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

    void setTour(vector<City>);

    vector<City>& getTour();

    double getFitness();

    double distance(const int,const int);

private:
    vector<City> tour;
};

#endif