#ifndef GPX2SUPPORT_HPP
#define GPX2SUPPORT_HPP

#include <string>
using std::string;

#include <utility>
using std::pair;

#include <vector>
using std::vector;

#include "GPX2Structs.hpp"
#include "Node.hpp"
#include "Partition.hpp"


/*  
-------------------------------------------------------------------------------------------------------------
    Utility Methods
    Methods that give support to the GPX steps
*/

namespace GPX2Support{
    // Checks if the partition is feasible
    bool checkPartition(Partition*, GPX2Structs::NodeMap&, GPX2Structs::NodeMap&);

    // After being found, the partitions may contain AcessNodes (nodes that link  partitions), if the AcessNode links the partion to itself, this node is removed from the AcessNode list, because in this case it isn't needeed for the GPX.
    void cleanInsideAccess(GPX2Structs::PartitionMap&, GPX2Structs::NodeMap&);

    // Functions to check the equality between two Pairs
    bool comparePairString(const pair<string, string>&, const pair<string, string>&);

    // Methods to clean the pointers and vectors used
    void deletePartitionMap(GPX2Structs::PartitionMap&);
    void deleteNodeMap(GPX2Structs::NodeMap&);

    // DFS inside of the partition to verify if the AcessNodes are connected and return the nodes that it went through
    pair<GPX2Structs::SearchResult, vector<string>> DFS_inside(string, string, GPX2Structs::NodeMap, Partition*);

    // DFS outside the partition to look for connections between partitions
    pair<GPX2Structs::SearchResult, vector<string>> DFS_outside(string, GPX2Structs::PartitionMap&, GPX2Structs::NodeMap&);

    // Erase a subvector from a vector
    void eraseSubVector(vector<string>&, vector<string>&);    

    //This method will look for in which partition the vertex is stored using Breadth-First Search, and returns a list of all nodes the composes the partition.
    vector<string> findPartition(const string,GPX2Structs::NodeMap&);
    
    // Function to obtain the entries and exits that are connected to a partition
    vector<pair<string, string>> getEntryAndExitList(Partition*, GPX2Structs::NodeMap&, GPX2Structs::NodeMap&);

    //This method inserts the Ghost node in the parent graph
    void insertGhost(string&, GPX2Structs::NodeMap&, Node*);

    vector<int> mapToTour(GPX2Structs::NodeMap&);

    // Partial distance, to check the best parent in each partiton
    int partialDistance(string, string, GPX2Structs::NodeMap, Partition*);

    // Prints the city map
    void printMap(GPX2Structs::NodeMap&, std::ostream&);

    // Remove the Ghosts nodes from the best child
    void removeGhosts(GPX2Structs::NodeMap&);
    
    // Gets the list of entry and exit from all partitions
    void setAllEntryAndExits(GPX2Structs::PartitionMap&, GPX2Structs::NodeMap&, GPX2Structs::NodeMap&);

    // Returns the distance of all the graph
    int totalDistance(GPX2Structs::NodeMap&);

    GPX2Structs::NodeMap tourToMap(vector<int>&);

    // Returns the ID of the partition that the city is contained
    int whichPartition(const string, GPX2Structs::PartitionMap);
};

void printMapAndPartitions(std::ostream& out, vector<int>, GPX2Structs::PartitionMap partitions, vector<GPX2Structs::Parent> partitionsChoosen);

#endif