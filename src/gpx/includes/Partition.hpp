#ifndef PARTITION_H
#define PARTITION_H

#include <ostream>
using std::ostream;

#include <string>
using std::string;

#include <utility>
using std::pair;

#include <vector>
using std::vector;


class Partition {

    friend ostream& operator<<(ostream&, const Partition&);

public:
    //Id of the connected partition and the number of times the partitions are connected - fusion
    using PartitionConnected = pair<int, int>;

    /*
        Struct to keep the informations about the connection of the partitions

        connectedPartition: connected partition ID 
        node: node that connected the partition to the other one
        connectedNode: node of the connected partition
    */
    using ConnectionNode = struct ConnectionNode {
        int connectedPartition;
        string node;
        string connectedNode;

        ConnectionNode(int cpart, string node, string cnode)
            : connectedPartition(cpart)
            , node(node)
            , connectedNode(cnode)
        {
        }
    };

    Partition();
    Partition(const int, vector<string>, vector<string>);

    vector<string>& getNodes();
    vector<string>& getAccessNodes();
    vector<ConnectionNode>& getConnectedTo();
    vector<PartitionConnected>& getConnections();
    vector<pair<string, string>> getEntryAndExits();
    int getId();

    void setId(const int);
    void setNodes(vector<string>&);
    void setAccessNodes(vector<string>&);
    void setConnectedTo(vector<ConnectionNode>&);
    void setConnections(vector<PartitionConnected>);
    void setEntryAndExits(vector<pair<string, string>>);

private:
    int id;
    vector<string> nodes;
    vector<string> accessNodes;
    vector<pair<string, string>> entryAndExits;

    //Var used inside fusion
    vector<ConnectionNode> connectedTo;
    vector<PartitionConnected> connections;
};

#endif