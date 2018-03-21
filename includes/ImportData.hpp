#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

#include "City.hpp"
#include "Map.hpp"
#include "Population.hpp"

using std::cout;
using std::ifstream;
using std::ostringstream;
using std::regex;
using std::string;
using std::to_string;
using std::vector;

class ImportData {
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

    void setEdge_Type(string);

    Population importFirstPopulation(Map, string, unsigned);

private:
    string tspName = "N/A";
    string type = "N/A";
    string comment = "";
    string edge_type = "N/A";
    string node_comment = "N/A";
    string flag;
    string flagaux;
    ifstream myfile;
    vector<City> citiescoord;
    int dimension;
    void regexManager(string);
    bool findIgnoredWords(string);
    void reader(string, string);
    std::vector<std::string> explode(std::string const & s, char delim);
};

#endif