#ifndef LOG_HPP
#define LOG_HPP

#include "Population.hpp"

#include <fstream>
using std::ofstream;

#include <ostream>
using std::ostream;

#include <vector>
using std::vector;

#include <string>
using std::string;

namespace Log{
    //Log functions
    ofstream* initLogFile();
    void printHeader(ostream&);
    void printFooter(ostream&,Population&,unsigned,unsigned);
    void printTime(ostream&,string,double);
    void printTourFile(vector<int>&,string,string);
};

#endif