#ifndef GPX2_HPP
#define GPX2_HPP

#include <algorithm>
#include <deque>
#include <fstream>
#include <iterator>
#include <map>
#include <set>
#include <string>

#include <iostream>

#include "City.hpp"
#include "Tour.hpp"
#include "CityNode.hpp"
#include "Partition.hpp"
#include "Utils.hpp"

using std::deque;
using std::find;
using std::make_pair;
using std::map;
using std::remove;
using std::set;
using std::size_t;
using std::string;
using std::to_string;

class GPX2 {

    // Mainly used data structures
    using PartitionMap = map<int, Partition*>;
    using CityMap = map<string, CityNode*>;

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

    // Enum that classifies the possibilites of the DFS return
    enum class SearchResult { CONNECTED_TO_PARTITION,
        CONNECTED_TO_SELF,
        IS_CONNECTED,
        IS_NOT_CONNECTED };

    // Enum to store the parents symbols
    enum class Parent { RED,
        BLUE };

    // -------------------------------------------------

    const string ghostToken = "-";
    friend bool operator==(const UnfeasibleConnection&, const UnfeasibleConnection&);

public:
    /*        
        GPX's Main method
        Receives two structures Tour, named as parents and returns a new Tour named child.
        All steps of GPX are done when ths method is called.
    */
    Tour static crossover(Tour, Tour);

private:
    GPX2();
    ~GPX2();

    /*  
        STEP 1 - Tour mapping

        The Tour structure, initially , it is a city list in which its order represents the city visitation order.
        To be possible of using GPX, it is necessary that the Tour be represented in the form of a graph, 
        in which the vertices are the cities, while the edges are the connections between the cities.

        The method receives a tour as parameter and generates a graph from it.
    */
    CityMap tourToMap(Tour&);

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 2 - Applicating GhostNodes

        The GPX uses the GhostNode concept,
        when two parent graphs are united, it may exist nodes of degree 4 , or in another words, nodes connected to 4 other vertices.
        These vertices can be duplicated, therefore,  a ghost node is created in the same spot of the "real" vertex, 
        by doing this is possible to increase the partition number.
        
        In theory the ghosts nodes should be added before the union of the parent graphs, however, to reduce the use of resources
        they are added before the union, doing so, when the graphs are united, the GhostNodes are already inserted in union graph.
    */
    //This metod verifies which nodes will become Ghosts and creates them
    void createGhosts();
    //This method inserts the Ghost node in the parent graph
    void insertGhost(string&, CityMap&, CityNode*);

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 3 -  Union of the Parent Graphs

        To be possible to create partitions it's needed that a Graph containing the union of the parents be generated, when the graphs are united, the resulting graph is an overlapping of the two parents. This graph will be called GU.
        
        This method joins Tour Red and Tour blue
    */
    void joinGraphs();

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 4 - Removing the overlapping edges

        After creating the GU it's necessary to remove the overlapping edges, that is, the edges which are in both parents. By doing theses "cuts" the partitions are created.

        The cuts will be described as cost 0 edges and also they will be marked as AcessNodes.

        This method does the cuts, creating the GU'.
    */
    void cutCommonEdges();

    // -----------------------------------------------------------------------------------------------------
    /* 
        STEP 5 - Finding the partitions
        
        When executing a GPX, after the creation of the GU' is needed to find "SubTours" that represent partitions, these partitions are used to create the children afterwards.

        The partitions are circuits with connections between the the vertices. The higher the partition number the better is the performance of the Crossover.
    */
    //This method will look for in which partition the vertex is stored, this method returns a list of all nodes the composes the partition.
    vector<string> findPartition(const string);
    //Does the previous verification for all the partitions
    void findAllPartitions();
    // After being found, the partitions may contain AcessNodes (nodes that link  partitions), if the AcessNode links the partion to itself, this node is removed from the AcessNode, because in this case it isn't needeed for the GPX.
    void cleanInsideAccess();

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 6 - Verify if the partitions are recombinants (feasible)

        To be possible of executing the GPX it's also needed that the partitions be recombinant, that is, that they have the same AcessNodes that their parents.
        This means that is possible to go throught the same subtour in both parents.
        If a partition does not follow this rule, so it is a non-recombinant partition, which we call unfeasible. In order to improve performance, the GPX2 try to fuse unfeasible partitions, so they might become feasible, hence, increasing the partition number.
    */
    // Checks if the partition is feasible
    bool checkPartition(Partition*);
    //Does the previous verification for all partitions
    void checkAllPartitions();
    // For the unfeasible partition, a fusion try is executed (the fusion methods are showed below)
    void fusion();
    // Gets the list of entry and exit from all partitions
    void setAllEntryAndExits();

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 7 - Choose the "SubTours" that will compound the child

        In this step it will be veryfied between the parents which of the two have the better SubTour to be inherited by the child. Each partition is made up by the union of a SubTour from each parent, the best SubTour is choose and saved.
    */
    void choose();

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 8 - Create the child graph

        After the choice of the best SubTour, the child map is created.
        After its creations it's necessary to remove the GhostNodes, which will be united again to ther origin vertices.
        
        For futher improvement, after the child assemblement, it's verified between the tours, which one has the better fitness, this one will be chose to be the child.
        
    */
    // This method will assemble the child's Graph from both parent Graphs, then it checks which one is better
    void buildOffspring();
    // Remove the Ghosts nodes from the best child
    void removeGhosts(CityMap&);

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 9 -Converting the graph to a Cities List (Tour)

        When the GPX is finished, the GPX will transform the child graph into a tour again.
    */
    Tour mapToTour(CityMap&);

    /*  
    -------------------------------------------------------------------------------------------------------------

        Utility Methods
        Methods that give support to the GPX steps
    */
    // Functions to check the equality between two Pairs
    bool comparePairInt(const UnfeasibleConnection&, const UnfeasibleConnection&);
    bool comparePairString(const pair<string, string>&, const pair<string, string>&);
    // Methods to clean the pointers and vectors used
    void static deletePartitionMap(PartitionMap&);
    // DFS outside the partition to look for connections between partitions
    pair<SearchResult, vector<string>> DFS_outside(string, PartitionMap, bool = false);
    // DFS inside of the partition to verify if the AcessNodes are connected
    pair<SearchResult, vector<string>> DFS_inside(string, string, CityMap, Partition*);
    // Erase a subvector from a vector
    void eraseSubVector(vector<string>&, vector<string>&);
    // Function to obtain the entries and exits that are connected to a partition
    vector<pair<string, string>> getEntryAndExitList(Partition*);
    // Partial distance, to check the best parent in each partiton
    int partialDistance(string, string, CityMap, Partition*);
    // Prints the city map
    void printMap(CityMap&, std::ostream&);
    // Returns the distance of all the graph
    int totalDistance(CityMap&);
    // Returns the ID of the partition that the city is contained
    int whichPartition(const string, PartitionMap);

    /*  
        Utility Methods- FUSION

        To execute the Fusion procedure is needed some steps and procedures
    */
    // Checks if all the partitions are unfesabile (in case of during a fusion execution a feasible partition is created)
    void checkUnfeasiblePartitions();
    // Counts how many connection there are amongst the partition
    void countConnectedPartitions();
    // Fuse the partitions
    void fusePartitions();
    // Returns a list of all the fusions in which the partition appears
    vector<UnfeasibleConnection> fusionsWithPartition(const int, vector<UnfeasibleConnection>&);
    // Generates a list of all fusions that will be realized, executing the necessary validations already 
    void generateFusionPairs();
    // Check the unfeasible partition that are connected
    bool unfeasiblePartitionsConnected();
    // Creates a list with all IDs from the partitions that can be fused 
    UnfeasibleConnection whichPartitionToFuseWith(Partition*);
    void deleteCityMap(CityMap&);

    /* 
        Variables
    */
    PartitionMap feasiblePartitions;
    PartitionMap unfeasiblePartitions;
    CityMap unitedGraph;
    vector<UnfeasibleConnection> fuseWith;

    CityMap red;
    CityMap blue;
    vector<Parent> partitionsChoosen;
    Parent offspringChoosen;
};

#endif