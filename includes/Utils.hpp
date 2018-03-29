#ifndef UTILS_HPP
#define UTILS_HPP

#include <utility> 
#include <cmath>
#include <string>
#include <iostream>
#include <vector>

int getFitness(const std::vector<int>&);
std::ostream& printTour(const std::vector<int>&, std::ostream&);

int distance(int,int);
int distance(std::string,std::string);

int EUC_2D(int,int);
int GEO(int,int);
int ATT(int,int);
#endif