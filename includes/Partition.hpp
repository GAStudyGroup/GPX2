#ifndef PARTITION_H
#define PARTITION_H

#include <set>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <tuple>

using std::string;
using std::vector;
using std::ostream;
using std::endl;
using std::pair;
using std::set;
using std::tuple;
using std::get;

class Partition {

    friend ostream& operator<<(ostream&, const Partition&);

public:
    // ID da partição conectada e o total de conexões entre elas
    using PartitionConnected = pair<int, int>;

    using ConnectionNode = struct ConnectionNode{
        int connectedPartition;
        string node;
        string connectedNode;

        ConnectionNode(int cpart, string node, string cnode):connectedPartition(cpart), node(node), connectedNode(cnode){}
    };

    Partition();
    Partition(const int, vector<string>, vector<string>);

    vector<string>& getNodes();
    vector<string>& getAccessNodes();
    vector<ConnectionNode>& getConnectedTo();
    vector<PartitionConnected>& getConnections();
    vector<pair<string,string>> getEntryAndExits();
    int getId();

    void setId(const int);
    void setNodes(vector<string>&);
    void setAccessNodes(vector<string>&);
    void setConnectedTo(vector<ConnectionNode>&);
    void setConnections(vector<PartitionConnected>);
    void setEntryAndExits(vector<pair<string,string>>);

private:
    int id;
    vector<string> nodes;
    vector<string> accessNodes;
    vector<pair<string,string>> entryAndExits;

    //variáveis apenas utilizadas na fusion
    vector<ConnectionNode> connectedTo;
    vector<PartitionConnected> connections;
};

#endif