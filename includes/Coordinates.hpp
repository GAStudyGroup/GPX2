#ifndef COORDINATES_HPP
#define COORDINATES_HPP

#include <algorithm>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>

#include "City.hpp"

using std::find_if;
using std::vector;
using std::string;
using std::transform;
using std::back_inserter;

class Coordinates {

public:
    Coordinates();
    Coordinates(vector<City>);
    vector<int> getCityList();
    void setCityList(vector<City>);
    City getCityById(const int);
    City getCityById(const string);

private:
    vector<City> cityList;
};

#endif