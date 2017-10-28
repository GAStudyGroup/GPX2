#ifndef PARTITION_H
#define PARTITION_H

#include <ostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::ostream;
using std::endl;

class Partition {

    friend ostream& operator<<(ostream&, const Partition&);

public:
    Partition();
    Partition(const int, vector<string>, vector<string>);

    vector<string>& getNodes();
    vector<string>& getAccessNodes();
    int getId();

    void setId(const int);
    void setNodes(vector<string>&);
    void setAccessNodes(vector<string>&);

private:
    int id;
    vector<string> nodes;
    vector<string> accessNodes;
};

#endif