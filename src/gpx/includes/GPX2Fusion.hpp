#ifndef GPX2FUSION_HPP
#define GPX2FUSION_HPP


#include <vector>
using std::vector;

#include "GPX2Structs.hpp"
#include "Partition.hpp"

/*  
    Utility Methods- FUSION
*/
namespace GPX2Fusion{
    // Checks if all the partitions are unfesabile (in case of during a fusion execution a feasible partition is created)
    void checkUnfeasiblePartitions(GPX2Structs::PartitionMap&,GPX2Structs::PartitionMap&, GPX2Structs::NodeMap&, GPX2Structs::NodeMap&);

    // Functions to check the equality between two Pairs
    bool comparePairInt(const GPX2Structs::UnfeasibleConnection&, const GPX2Structs::UnfeasibleConnection&);

    bool compareUnfeasibleConnections(const GPX2Structs::UnfeasibleConnection&, const GPX2Structs::UnfeasibleConnection&);

    // Counts how many connection there are amongst the partition
    void countConnectedPartitions(GPX2Structs::PartitionMap&);

    // Fuse the partitions
    void fusePartitions(vector<GPX2Structs::UnfeasibleConnection>&,GPX2Structs::PartitionMap&, GPX2Structs::NodeMap&, GPX2Structs::NodeMap&,GPX2Structs::NodeMap&);

    // Returns a list of all the possible fusions in which the partition appears
    vector<GPX2Structs::UnfeasibleConnection> fusionsWithPartition(const int, vector<GPX2Structs::UnfeasibleConnection>&);

    // Generates a list of all fusions that will be realized, executing the necessary validations already 
    void generateFusionPairs(vector<GPX2Structs::UnfeasibleConnection>&,GPX2Structs::PartitionMap&);

    // Check that unfeasible partitions that are connected
    bool unfeasiblePartitionsConnected(GPX2Structs::PartitionMap&, GPX2Structs::NodeMap&);

    // Creates a list with all IDs from the partitions that can be fused 
    GPX2Structs::UnfeasibleConnection whichPartitionToFuseWith(Partition*);
};

#endif