#ifndef COORDINATES_HPP
#define COORDINATES_HPP

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "City.hpp"

using std::find_if;
using std::vector;

class Coordinates {

public:
    Coordinates();
    Coordinates(vector<City>);
    vector<City> getCityList();
    void setCityList(vector<City>);
    City getCityById(const int);

private:
    vector<City> cityList;
};

#endif