#ifndef OPT_HPP
#define OPT_HPP

#include "Utils.hpp"

#include <vector>
#include <utility>

using std::vector;
using std::pair;

class Opt{
public:
    vector<int> static optimize(vector<int>&);

private:
    using edge = pair<int,int>;
    Opt();
    ~Opt();
    bool adjacent(edge&,edge&);
    bool isBetter(edge&,edge&,vector<int>&);
};

#endif