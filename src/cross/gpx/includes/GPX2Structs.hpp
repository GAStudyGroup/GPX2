#ifndef GPX2STRUCTS_HPP
#define GPX2STRUCTS_HPP

#include <map>
using std::map;

#include <string>
using std::string;

#include "Node.hpp"
#include "Partition.hpp"

namespace GPX2Structs{
    //ghost token used in GPX2
    const string ghostToken = "-";

    // Mainly used data structures
    using PartitionMap = map<int, Partition*>;
    using NodeMap = map<string, Node*>;

    /*
        Structure to store info about connecions from unfeasible partitions
        partitionId1
        partitionId2
        numberOfConnections: number of connections amongst the two partiitons
    */
    using UnfeasibleConnection = struct UnfeasibleConnection {
        int partitionId1;
        int partitionId2;
        int numberOfConnections;

        UnfeasibleConnection(int id1, int id2, int number)
            : partitionId1(id1)
            , partitionId2(id2)
            , numberOfConnections(number)
        {
        }
    };
    // Enum that classifies the possibilites of the DFS(Depth First Searh) return
    enum class SearchResult { CONNECTED_TO_PARTITION,
        CONNECTED_TO_SELF,
        IS_CONNECTED,
        IS_NOT_CONNECTED };

    // Enum to store the parents symbols
    enum class Parent { RED, BLUE };
};

#endif