#ifndef PARTITION_H
#define PARTITION_H

#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using std::string;
using std::vector;
using std::ostream;
using std::endl;
using std::pair;
using std::set;

class Partition {

    friend ostream& operator<<(ostream&, const Partition&);

public:
    using ConnectionPairVector = vector<pair<int, int>>;
    //id da partição conectada, id do nó que liga e o id do nó que está ligado
    using ConnectedNode = vector<pair<int, pair<string, string>>>;

    Partition();
    Partition(const int, vector<string>, vector<string>);

    vector<string>& getNodes();
    vector<string>& getAccessNodes();
    ConnectedNode& getConnectedTo();
    ConnectionPairVector& getConnections();
    int getId();

    void setId(const int);
    void setNodes(vector<string>&);
    void setAccessNodes(vector<string>&);
    void setConnectedTo(ConnectedNode&);
    void setConnections(ConnectionPairVector);

private:
    int id;
    vector<string> nodes;
    vector<string> accessNodes;

    //connected to partition int in the node string
    ConnectedNode connectedTo;

    ConnectionPairVector connections;
};

#endif