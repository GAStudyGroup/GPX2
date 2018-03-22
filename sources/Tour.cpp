#include "Tour.hpp"

#include <iostream>

Tour::Tour() {}

Tour::Tour(vector<City> route)
    : route(route)
{
}

void Tour::setRoute(vector<City> route)
{
    this->route = route;
}

vector<City>& Tour::getRoute()
{
    return route;
}

int Tour::getFitness()
{ // Irá retornar a fitness do Tour específico
    int sum{ 0 };
    unsigned size = route.size();
    for(unsigned i=0;i<size;i++){
        pair<double,double> p1(route[i].getX(),route[i].getY()),p2(route[(i+1)%size].getX(),route[(i+1)%size].getY());
        sum += distance(p1,p2);
    }
    return (sum);
}

ostream& operator<<(ostream& output, Tour& t)
{ // Overload de operador para impressão da população
    output<<"===================================================\n";
    for (City c : t.getRoute()) {
        output << c << "\n";
    }
    output << "Fitness: " << t.getFitness() << "\n";
    return (output);
}