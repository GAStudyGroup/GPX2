#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "City.hpp"
#include "Population.hpp"

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

    Population importFirstPopulation(string, unsigned);

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
    vector<string> explode(string const & s, char delim);
};

#endif