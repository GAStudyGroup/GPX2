#ifndef NODE_HPP
#define NODE_HPP

#include <ostream>
using std::ostream;

#include <string>
using std::string;

#include <utility>
using std::pair;

#include <vector>
using std::vector;

class Node {

    friend ostream& operator<<(ostream&, Node&);

public:
    //string is the ID of the connected edge and the double is the distance to that node
    using node = pair<string, double>;

    Node(string);

    string getId() const;
    vector<node>& getEdges();
    bool getAccess() const;

    void setId(string);
    void setEdges(vector<node>);
    void setAccess(bool);

    void addEdge(node);
    void deleteEdge(unsigned);
    bool operator==(const Node&) const;

private:
    string id;
    vector<node> edges;
    bool access;
};

#endif