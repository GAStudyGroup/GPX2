#ifndef CITYMAP__HPP
#define CITYMAP__HPP

#include <map>
#include <string>
#include <deque>
#include <algorithm>
#include <fstream>

#include "CityNode.hpp"
#include "Tour.hpp"

using std::string;
using std::deque;
using std::map;
using std::to_string;
using std::find;

using CityMap = map<string, CityNode*>;

CityMap tourToMap(Tour&);

Tour mapToTour(CityMap&);

double distance(const CityNode&, const CityNode&);

void printMap(CityMap&, std::ostream&);

void deleteCityMap(CityMap&);

#endif