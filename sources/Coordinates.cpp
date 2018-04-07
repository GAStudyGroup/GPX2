#include "Coordinates.hpp"

#include <algorithm>
using std::find_if;
using std::transform;

#include <iterator>
using std::back_inserter;

Coordinates::Coordinates() {}

Coordinates::Coordinates(vector<City> data)
    : cityList(data)
{
}

vector<int> Coordinates::getCityList()
{
    vector<int> tmp;
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
        return (City());
    }
}