#ifndef MAP_HPP
#define MAP_HPP

#include <cstdlib>
#include <vector>
#include "City.hpp"
using std::vector;

class Map{

public:
    Map();
    Map(vector<City>);
    vector<City>& getCityList();
    void setCityList(vector<City>);
private:
    vector<City> cityList;
};

#endif