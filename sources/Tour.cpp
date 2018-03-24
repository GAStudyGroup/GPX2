#include "Tour.hpp"
#include "Config.hpp"

#include <iostream>

Tour::Tour() {}

Tour::Tour(vector<City> route){
    for(City c : route){
        this->route.push_back(c.getId());
    }
}

Tour::Tour(vector<int> route):route(route){}

void Tour::setRoute(vector<int> route)
{
    this->route = route;
}

vector<int>& Tour::getRoute()
{
    return route;
}

int Tour::getFitness()
{ // Irá retornar a fitness do Tour específico
    int sum{0};
    unsigned size{(unsigned)route.size()};
    for(unsigned i=0; i<size; i++){
        sum += distance(route[i], route[(i+1)%size]);
    }
    return(sum);
}

ostream& operator<<(ostream& output, Tour& t)
{ // Overload de operador para impressão da população
    output<<"===================================================\n";
    for (int cityId : t.getRoute()) {
        output << cityId << " ";
    }
    output << "\nFitness: " << t.getFitness() << "\n";
    return (output);
}