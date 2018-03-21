#ifndef TOUR_HPP
#define TOUR_HPP

#include <cmath>
#include <vector>

#include "City.hpp"
#include "Utils.hpp"

using std::ostream;
using std::round;
using std::vector;

class Tour {

    friend ostream& operator<<(ostream&, Tour&);

public:
    Tour(); 
    Tour(vector<City>);

    void setRoute(vector<City>);

    vector<City>& getRoute();

    int getFitness();
private:
    vector<City> route;
};

#endif