#ifndef CITY_HPP
#define CITY_HPP

#include <ostream>
using std::ostream;

class City {

    friend ostream& operator<<(ostream&, const City&);

public:
    City();
    City(const int, const double, const double);
    int getId() const;
    double getX() const;
    double getY() const;
    void setId(const int);
    void setX(const double);
    void setY(const double);

    //overload do operador de comparação
    bool operator==(const City&) const;
    const City& operator=(const City&);

private:
    int id;
    double x;
    double y;
};

#endif