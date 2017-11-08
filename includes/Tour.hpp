#ifndef TOUR_HPP
#define TOUR_HPP

#include <cmath>
#include <iostream>
#include <vector>
#include "City.hpp"

using std::vector;
using std::ostream;
using std::setfill;
using std::setw;

class Tour {

    friend ostream& operator<<(ostream&,Tour &);    

public:
    Tour();
    Tour(vector<City>);

    void setRoute(vector<City>);

    vector<City>& getRoute();

    double getFitness();

    double distance(const int, const int);

private:
    vector<City> route;
};

#endif