#include "Map.hpp"

Map::Map(){}

Map::Map(vector<City> data):cityList(data){}

vector<City>& Map::getCityList(){
    return(cityList);
}

void Map::setCityList(vector<City> cityList){
    this->cityList = cityList;
}

City Map::getCityById(const int id){ 
    auto it = find_if(cityList.begin(),cityList.end(),[&id](const City &obj){return(obj.getId()==id);});
    if(it!=cityList.end()){
        return(*it);
    }else{
        //tirar warning
        //retorna cidade com id 0
        return(City());
    }
}