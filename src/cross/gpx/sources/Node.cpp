#include "Node.hpp"

Node::Node(string id)
    : id(id)
{
}

string Node::getId() const { return id; }
vector<Node::node>& Node::getEdges() { return edges; }
bool Node::getAccess() const { return access; }

void Node::setId(string id) { this->id = id; }
void Node::setEdges(vector<node> edges) { this->edges = edges; }
void Node::setAccess(bool access) { this->access = access; }


void Node::addEdge(node e)
{
    edges.push_back(e);
}
void Node::deleteEdge(unsigned i)
{
    if (i <= edges.size()) {
        edges.erase(edges.begin() + i);
    } else {
        exit(EXIT_FAILURE);
    }
}


bool Node::operator==(const Node& c) const
{
    return ((c.getId().compare(this->id) == 0) ? true : false);
}

ostream& operator<<(ostream &output, Node &cn){
    output<<"=====\nID: "<<cn.getId()<<"\n";
    output<<"edges: \n";
    for(Node::node n : cn.getEdges()){
        output<<"\tid "<<n.first<<" | dist: "<<n.second<<"\n";
    }
    output<<"is access: "<<cn.getAccess();
    output.flush();
    return(output);
}