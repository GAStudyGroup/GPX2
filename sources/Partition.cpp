#include "Partition.hpp"

Partition::Partition(){};
Partition::Partition(const int id, vector<string> nodes, vector<string> accessNodes)
    : id(id)
    , nodes(nodes)
    , accessNodes(accessNodes){};

vector<string>& Partition::getNodes() { return (nodes); }
vector<string>& Partition::getAccessNodes() { return (accessNodes); }
int Partition::getId() { return (id); }

void Partition::setId(const int id)
{
    this->id = id;
}

void Partition::setNodes(vector<string>& nodes)
{
    this->nodes = nodes;
}

void Partition::setAccessNodes(vector<string>& accessNodes)
{
    this->accessNodes = accessNodes;
}

ostream& operator<<(ostream& output, const Partition& partition)
{
    output << "Partition " << partition.id << endl;
    output << "Nodes: ";
    for (string i : partition.nodes) {
        output << i << " ";
    }
    output << endl;
    output << "Access List: ";
    for (string i : partition.accessNodes) {
        output << i << " ";
    }
    output << endl;
    return (output);
}