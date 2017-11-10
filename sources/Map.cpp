#include "Map.hpp"

Map::Map(){}

Map::Map(vector<City> data):cityList(data){}

vector<City>& Map::getCityList(){
    return(cityList);
}

void Map::setCityList(vector<City> cityList){
    this->cityList = cityList;
}