#include "Partition.hpp"

Partition::Partition(){};
Partition::Partition(const int id, vector<string> nodes, vector<string> accessNodes)
    : id(id)
    , nodes(nodes)
    , accessNodes(accessNodes){};

vector<string>& Partition::getNodes() { return (nodes); }
vector<string>& Partition::getAccessNodes() { return (accessNodes); }
Partition::ConnectionPairVector& Partition::getConnections(){ return(connections); }
int Partition::getId() { return (id); }

Partition::ConnectedNode& Partition::getConnectedTo(){return connectedTo;}

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

void Partition::setConnections(ConnectionPairVector){
    this->connections = connections;
}

void Partition::setConnectedTo(ConnectedNode &connectedTo){this->connectedTo = connectedTo;}

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
    output<<"connectedTo: \n";
    for(auto a : partition.connectedTo){
        output<<"partition "<<a.first<<" by node "<<a.second.first<<" wity "<<a.second.second<<"\n";
    }
    output<<"connections: \n";
    for(auto a : partition.connections){
        output<<a.first<<" with "<<a.second<<"\n";
    }
    output.flush();
    return (output);
}