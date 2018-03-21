#include "Tour.hpp"

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
    for (unsigned i = 0; i < (route).size(); i++) {
        if ((i + 1) == (route).size()) {
            pair<double,double> p1(route[i].getX(),route[i].getY()),p2(route[0].getX(),route[0].getY());
            sum += distance(p1,p2);
        } else {
            pair<double,double> p1(route[i].getX(),route[i].getY()),p2(route[i+1].getX(),route[i+1].getY());
            sum += distance(p1,p2);
        }
    }
    return (sum);
}

ostream& operator<<(ostream& output, Tour& t)
{ // Overload de operador para impressão da população

    for (City c : t.getRoute()) {
        output << c << "\n";
    }
    output << "Fitness: " << t.getFitness() << "\n";
    return (output);
}