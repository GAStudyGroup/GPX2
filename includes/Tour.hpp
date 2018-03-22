#ifndef TOUR_HPP
#define TOUR_HPP

#include <cmath>
#include <vector>

#include "Utils.hpp"

using std::ostream;
using std::round;
using std::vector;

class Tour {

    friend ostream& operator<<(ostream&, Tour&);

public:
    Tour(); 
    Tour(vector<City>);
    Tour(vector<int>);

    void setRoute(vector<int>);

    vector<int>& getRoute();

    int getFitness();
private:
    vector<int> route;
};

#endif