#include "City.hpp"


// Construtores
City::City(){
    this->id = 0;
    this->x = 0;
    this->y = 0;
}

City::City(const int id, const double x, const double y):id(id),x(x),y(y){}

// Getters and Setters

double City::getX() const{return(x);}

double City::getY() const{return(y);}

int City::getId() const{return id;}

void City::setX(const double x){this->x = x;}

void City::setY(const double y){this->y = y;}

void City::setId(const int id){this->id = id;}

// Overload de operadores

bool City::operator==(const City &c) const{  // Overload para comparação de city==city
    if((c.getX() == this->x) && (c.getY() == this->y) && (c.getId() == this->id)){
        return(true);
    }else{
        return(false);
    }
}

ostream& operator<<(ostream &output,const City &c){  // Overload para impressão da população
    //output<<setfill(' ')<<"id: "<<setw(6)<<c.getId()<<" x: "<<setw(6)<<c.getX()<<" y: "<<setw(6)<<c.getY();
    output<<c.id;
    return(output);
}

const City& City::operator=(const City &c){  // Overload de atribuição city=city
    this->x = c.getX();
    this->y = c.getY();
    this->id = c.getId();
    return(c);
}
