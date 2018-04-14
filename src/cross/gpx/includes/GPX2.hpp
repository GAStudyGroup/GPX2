#ifndef GPX2_HPP
#define GPX2_HPP


#include <string>
using std::string;

#include <vector>
using std::vector;

#include "GPX2Structs.hpp"

class GPX2 {

public:
    /*        
        GPX's Main method
        Receives two Tour structures, named as parents and returns a new Tour named child.
        All steps of GPX are done when ths method is called.
    */
    vector<int> static crossover(vector<int>, vector<int>);

private:
    GPX2();
    ~GPX2();

    /*  
        STEP 1 - Tour mapping

        The Tour structure, initially , it is a int list of ids in which its order represents the city visitation order.
        To be possible of using GPX, it is necessary that the Tour be represented in the form of a graph, 
        in which the vertices are the cities, while the edges are the connections between the cities.

        The method receives a tour as parameter and generates a graph from it.
    */
    void convertParentsToMap(vector<int>&,vector<int>&);

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 2 - Applicating GhostNodes

        The GPX uses the GhostNode concept,
        when two parent graphs are united, it may exist nodes of degree 4 , or in another words, nodes connected to 4 other vertices.
        These vertices can be duplicated, therefore,  a ghost node is created in the same spot of the "real" vertex, 
        by doing this is possible to increase the partition number.
        
        In theory the ghosts nodes should be added after the union of the parent graphs, however, to reduce the use of resources
        they are added before the union, doing so, when the graphs are united, the GhostNodes are already inserted in union graph.
    */
    //This metod verifies which nodes will become Ghosts and creates them
    void createGhosts();

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
    //Does the previous verification for all the partitions
    void findAllPartitions();

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 6 - Verify if the partitions are recombinants (feasible)

        To be possible of executing the GPX it's also needed that the partitions be recombinant, that is, that they have the same AcessNodes that their parents.
        #revisar depois / This means that is possible to go throught the same subtour in both parents.
        If a partition does not follow this rule, so it is a non-recombinant partition, which we call unfeasible. In order to improve performance, the GPX2 try to fuse unfeasible partitions, so they might become feasible, hence, increasing the partition number.
    */
    //Does the previous verification for all partitions
    void checkAllPartitions();
    // For the unfeasible partition, a fusion try is executed (the fusion methods are showed below)
    void fusion();
    

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

    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 9 -Converting the graph to a Cities List (Tour)

        When the GPX is finished, the GPX will remove the ghosts from the child and turn and tour.
    */
    vector<int> convertChoosenChild();
    
    /* 
        Variables
    */
    GPX2Structs::PartitionMap candidatePartitions;
    GPX2Structs::PartitionMap feasiblePartitions;
    GPX2Structs::PartitionMap unfeasiblePartitions;
    GPX2Structs::NodeMap unitedGraph;
    vector<GPX2Structs::UnfeasibleConnection> fuseWith;

    GPX2Structs::NodeMap red;
    GPX2Structs::NodeMap blue;
    vector<GPX2Structs::Parent> partitionsChoosen;
    GPX2Structs::Parent offspringChoosen;
};

#endif