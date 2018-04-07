#ifndef COORDINATES_HPP
#define COORDINATES_HPP

#include <vector>
using std::vector;

#include <string>
using std::string;

#include "City.hpp"


class Coordinates {

public:
    Coordinates();
    Coordinates(vector<City>);
    vector<int> getCityList();
    void setCityList(vector<City>);
    City getCityById(const int);

private:
    vector<City> cityList;
};

#endif