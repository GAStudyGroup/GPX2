#ifndef OPT_HPP
#define OPT_HPP

#include "Tour.hpp"
#include "CityNode.hpp"

#include <utility>

using std::pair;

class Opt{
public:
    Tour static optimize(Tour&);

private:
    using edge = pair<int,int>;
    Opt();
    ~Opt();
    bool adjacent(edge&,edge&);
    bool isBetter(edge&,edge&,Tour&);
};

#endif