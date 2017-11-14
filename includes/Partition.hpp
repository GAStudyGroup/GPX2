#ifndef PARTITION_H
#define PARTITION_H

#include <ostream>
#include <string>
#include <utility>
#include <vector>

using std::ostream;
using std::pair;
using std::string;
using std::vector;

class Partition {

    friend ostream& operator<<(ostream&, const Partition&);

public:
    // ID da partição conectada e o total de conexões entre elas
    using PartitionConnected = pair<int, int>;

    /*
        Estrutura para guardar as informações de conexão entre a partição do objeto e uma outra

        connectedPartition: ID da partição conectada
        node: nó da partição do objeto que se conecta
        connectedNode: nó da outra partição que está conectada
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

    //variáveis apenas utilizadas na fusion
    vector<ConnectionNode> connectedTo;
    vector<PartitionConnected> connections;
};

#endif