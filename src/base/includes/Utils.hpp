#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
using std::string;

#include <ostream>
using std::ostream;

#include <vector>
using std::vector;

int getFitness(const vector<int>&);
ostream& printTour(const vector<int>&, ostream&);

int distance(int,int);
int distance(string,string);

int EUC_2D(int,int);
int GEO(int,int);
int ATT(int,int);
#endif