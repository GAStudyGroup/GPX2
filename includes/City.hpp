#ifndef CITY_H
#define CITY_H

class City {
public:
    City();
    City(int, double, double);

    void setId(int);
    void setX(double);
    void setY(double);

    int getId();
    double getX();
    double getY();

private:
    int id;
    double x;
    double y;
};

#endif