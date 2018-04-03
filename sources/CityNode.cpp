#include "CityNode.hpp"

CityNode::CityNode(string id)
    : id(id)
{
}

string CityNode::getId() const { return id; }
vector<CityNode::node>& CityNode::getEdges() { return edges; }
bool CityNode::getAccess() const { return access; }

void CityNode::setId(string id) { this->id = id; }
void CityNode::setEdges(vector<node> edges) { this->edges = edges; }
void CityNode::setAccess(bool access) { this->access = access; }


void CityNode::addEdge(node e)
{
    edges.push_back(e);
}
void CityNode::deleteEdge(unsigned i)
{
    if (i <= edges.size()) {
        edges.erase(edges.begin() + i);
    } else {
        exit(EXIT_FAILURE);
    }
}


bool CityNode::operator==(const CityNode& c) const
{
    return ((c.getId().compare(this->id) == 0) ? true : false);
}

ostream& operator<<(ostream &output, CityNode &cn){
    output<<"=====\nID: "<<cn.getId()<<"\n";
    output<<"edges: \n";
    for(CityNode::node n : cn.getEdges()){
        output<<"\tid "<<n.first<<" | dist: "<<n.second<<"\n";
    }
    output<<"is access: "<<cn.getAccess();
    output.flush();
    return(output);
}