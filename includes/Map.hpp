#ifndef MAP_HPP
#define MAP_HPP

#include <cstdlib>
#include <vector>
#include <algorithm> 
#include "City.hpp"
using std::vector;
using std::find_if;

class Map{

public:
    Map();
    Map(vector<City>);
    vector<City>& getCityList();
    void setCityList(vector<City>);
    City getCityById(const int);
private:
    vector<City> cityList;
};

#endif