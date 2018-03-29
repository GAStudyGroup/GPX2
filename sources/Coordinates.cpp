#include "Coordinates.hpp"

Coordinates::Coordinates() {}

Coordinates::Coordinates(vector<City> data)
    : cityList(data)
{
}

vector<int> Coordinates::getCityList()
{
    vector<int> tmp;
    /* for(City c : cityList){
        tmp.push_back(c.getId());
    } */
    transform(cityList.begin(),cityList.end(),back_inserter(tmp),[](const City &c){return c.getId();});
    return (tmp);
}

void Coordinates::setCityList(vector<City> cityList)
{
    this->cityList = cityList;
}

City Coordinates::getCityById(const int id)
{
    auto it = find_if(cityList.begin(), cityList.end(), [&id](const City& obj) { return (obj.getId() == id); });
    if (it != cityList.end()) {
        return (*it);
    } else {
        //tirar warning
        //retorna cidade com id 0
        return (City());
    }
}