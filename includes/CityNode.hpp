#ifndef CITYNODE_HPP
#define CITYNODE_HPP

#include <string>
#include <utility>
#include <vector>

#include "City.hpp"

using std::pair;
using std::string;
using std::vector;

class CityNode {
public:
    //deixando o typedef assim então
    //int seria o ID, usando o id no map, podemos ir direto para o objeto
    using node = pair<string, double>;

    CityNode(string);

    string getId() const;
    vector<node>& getEdges();
    bool getAccess() const;

    void setId(string);
    void setEdges(vector<node>);
    void setAccess(bool);

    void addEdge(node);
    void deleteEdge(unsigned);
    bool operator==(const CityNode&) const;

private:
    string id;
    vector<node> edges;
    bool access;
};

#endif