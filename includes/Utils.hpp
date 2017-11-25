#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <unordered_map>
#include <utility>
#include <cmath>

using std::vector;
using std::unordered_map;
using std::pair;
using std::round;
using std::sqrt;
using std::pow;


double getFitness(vector<int>, unordered_map<int,pair<double,double>> &);
double distance(int,int, unordered_map<int,pair<double,double>> &);

#endif