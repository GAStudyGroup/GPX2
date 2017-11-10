#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <stdlib.h>
#include "City.hpp"
#include "Population.hpp"
#include "Map.hpp"

using std:: ifstream;
using std:: string;
using std:: cout;
using std:: regex;
using std:: vector;
using std:: ostringstream;
using std::to_string;


class ImportData{
public:
   ImportData(string);
   vector<City> getCitiesCoord();
   string gettspName();
   string getType();
   string getedge_type();
   string getnode_comment();
   string getcomment();
   void printInfos();
   string getInfos();

   Population importFirstPopulation(Map,string,unsigned);
private:
   string tspName="N/A";
   string type="N/A";
   string comment="";
   string edge_type="N/A";
   string node_comment="N/A";
   string flag;
   string flagaux;
   ifstream myfile;
   vector<City> citiescoord;
   int dimension;
   void regexManager(string);
   bool findIgnoredWords(string);
   void reader(string, string);
};

#endif