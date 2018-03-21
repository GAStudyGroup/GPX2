#ifndef UTILS_HPP
#define UTILS_HPP

#include <utility> 
#include <cmath>

#include "CityNode.hpp"

/* int distance(const CityNode&, const CityNode&); */
/* template <class c>
int distance(const c& c1,const c& c2){
    return (round(sqrt(pow((c1.getX() - c2.getX()), 2) + pow((c1.getY() - c2.getY()), 2))));
} */

int distance(std::pair<double,double>,std::pair<double,double>);

int EUC_2D(std::pair<double,double>,std::pair<double,double>);
int GEO(std::pair<double,double>,std::pair<double,double>);
int ATT(std::pair<double,double>,std::pair<double,double>);
#endif