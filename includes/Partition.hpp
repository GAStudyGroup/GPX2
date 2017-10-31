#ifndef PARTITION_H
#define PARTITION_H

#include <ostream>
#include <string>
#include <vector>
#include <utility>

using std::string;
using std::vector;
using std::ostream;
using std::endl;
using std::pair;

class Partition {

    friend ostream& operator<<(ostream&, const Partition&);

public:
    Partition();
    Partition(const int, vector<string>, vector<string>);

    vector<string>& getNodes();
    vector<string>& getAccessNodes();
    vector<pair<int,string>>& getConnectedTo();
    int getId();

    void setId(const int);
    void setNodes(vector<string>&);
    void setAccessNodes(vector<string>&);
    void setConnectedTo(vector<pair<int,string>>&);

private:
    int id;
    vector<string> nodes;
    vector<string> accessNodes;

    //connected to partition int in the node string
    vector<pair<int,string>> connectedTo;
};

#endif