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
            sum += distance(i, 0);
        } else {
            sum += distance(i, i + 1);
        }
    }
    return (sum);
}

double Tour::distance(const int a, const int b)
{ // Retorno da distancia entre duas cidades adjacentes
    return (round(sqrt(pow(((route)[a].getX()) - ((route)[b].getX()), 2) + pow(((route)[a].getY()) - ((route)[b].getY()), 2))));
}

ostream& operator<<(ostream& output, Tour& t)
{ // Overload de operador para impressão da população

    for (City c : t.getRoute()) {
        output << c << "\n";
    }
    output << "Fitness: " << t.getFitness() << "\n";
    return (output);
}