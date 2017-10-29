#include "City.hpp"

City::City(){};

City::City(int id, double x, double y)
    : id(id)
    , x(x)
    , y(y)
{
}

void City::setId(int id) { this->id = id; }
void City::setX(double x) { this->x = x; }
void City::setY(double y) { this->y = y; }

int City::getId() { return (id); }
double City::getX() { return (x); }
double City::getY() { return (y); }