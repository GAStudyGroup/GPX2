#include "CityNode.hpp"

CityNode::CityNode(string id, double x, double y)
    : id(id)
    , x(x)
    , y(y)
{
}

string CityNode::getId() const { return id; }
double CityNode::getX() const { return x; }
double CityNode::getY() const { return y; }

vector<CityNode::node>& CityNode::getEdges() { return edges; }

bool CityNode::getAccess() const { return access; }

void CityNode::setId(string id) { this->id = id; }
void CityNode::setX(double x) { this->x = x; }
void CityNode::setY(double y) { this->y = y; }

void CityNode::setEdges(vector<node> edges) { this->edges = edges; }

void CityNode::setAccess(bool access) { this->access = access; }

void CityNode::addEdge(node e)
{
    edges.push_back(e);
}

void CityNode::deleteEdge(int i)
{
    if (i <= edges.size()) {
        edges.erase(edges.begin() + i);
    } else {
        cout << "ta apagando fora do limite pratinha" << endl;
    }
}

bool CityNode::operator==(const CityNode& c) const
{
    return ((c.getId().compare(this->id) == 0) ? true : false);
}
