#include "GPX2.hpp"
#include "Utils.hpp"
#include "GPX2Support.hpp"
#include "GPX2Fusion.hpp"

#include "Log.hpp"

#include <set>
using std::set;

#include <algorithm>
using std::remove;

#include <utility>
using std::make_pair;

#include <iostream>
using std::cout;
using std::endl;

vector<int> GPX2::crossover(vector<int> redT, vector<int> blueT)
{
    GPX2 obj;
    // Step 1
    obj.convertParentsToMap(redT,blueT);

    // Step 2
    obj.createGhosts();

    // Step 3
    obj.joinGraphs();

    // Step 4
    obj.cutCommonEdges();

    // Step 5
    obj.findAllPartitions();

    //the tours are equal if 0, or one of them is better if 
    if (obj.candidatePartitions.size() < 2) {
        return ((getFitness(redT) < getFitness(blueT)) ? redT : blueT);
    }

    // Step 6
    obj.checkAllPartitions();

    // Fusion
    // It's needed to have more than one unfeasible partition to fuse them
    if (obj.unfeasiblePartitions.size() > 1) {
        obj.fusion();
    }

    if (obj.feasiblePartitions.size() < 1) {
        return ((getFitness(redT) < getFitness(blueT)) ? redT : blueT);
    }

    // Step 7
    obj.choose();

    // Step 8
    obj.buildOffspring();


    // Step 9
    //return obj.convertChoosenChild();
    vector<int> offspring{obj.convertChoosenChild()};
    if(getFitness(redT) > getFitness(offspring) && getFitness(blueT) > getFitness(offspring) ){
        Log::printTourFile(redT,"red");
        Log::printTourFile(blueT,"blue");
        Log::printTourFile(offspring,"off");
    }
    return offspring;
}

// -----------------------------------------------------------------------------

// STEP 1 - Tour Mapping
void GPX2::convertParentsToMap(vector<int>& redT,vector<int>& blueT){
    red = GPX2Support::tourToMap(redT);
    blue = GPX2Support::tourToMap(blueT);
}

// -----------------------------------------------------------------------------

// STEP 2 - Create and insert ghosts
void GPX2::createGhosts()
{
    set<string> ghosts;
    vector<string> ghostsToAdd;

    // The SET container does not allow repeated values inside himself
    for (auto& city : red) {
        string idKey = city.first;
        for (unsigned i = 0; i < 2; i++) {
            ghosts.insert(city.second->getEdges()[i].first);
            ghosts.insert(blue[idKey]->getEdges()[i].first);
        }
        if (ghosts.size() == 4) { // node with degree 4
            //mark the node to transform in ghost node
            ghostsToAdd.push_back(idKey);
        }
        ghosts.clear();
    }

    for(string id : ghostsToAdd){
        //the ghost id is created adding a token to the normal ID
        string ghostID = id+GPX2Structs::ghostToken;

        Node* ghostNode = new Node(ghostID);
        GPX2Support::insertGhost(id, red, ghostNode);

        ghostNode = new Node(ghostID);
        GPX2Support::insertGhost(id, blue, ghostNode); 
    }
}

// -----------------------------------------------------------------------------

// STEP 3 - Creating the GU

void GPX2::joinGraphs()
{ // joins the two graphs, creating the Gu

    vector<string> cityList;
    for (auto node : red) {
        cityList.push_back(node.first);
    }

    for (string id : cityList) {
        Node* c = red[id];
        // creates the entry in the union map
        unitedGraph.insert(
            make_pair((c->getId()), new Node((c->getId()))));
        // puts the edges in the union/*  */ map
        for (Node::node n : red[(c->getId())]->getEdges()) {
            unitedGraph[(c->getId())]->addEdge(make_pair(n.first, n.second));
        }
        for (Node::node n : blue[(c->getId())]->getEdges()) {
            unitedGraph[(c->getId())]->addEdge(make_pair(n.first, n.second));
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 4 - Creating the GU' (cutting the common edges)

void GPX2::cutCommonEdges()
{ // executes the process of cutting the common edges between the parents from the union graph
    for (auto &c : unitedGraph) { // goes by all entres from Gu
        vector<Node::node>& vec = c.second->getEdges(); //Loads the vector with the entries contained in the map entry
        for (unsigned i = 0; i < vec.size(); i++) { // goes by the edges' vector
            int commonEdges{ 0 }; //Counts the common edges, in case of two, both are "cutted"
            int last{ -1 }; // position of the first instance of the common edge
            for (unsigned j = 0; j < vec.size(); j++) { // goes by the edges's vector
                if (vec[i].first == vec[j].first) { // Checks the equality
                    commonEdges++;
                    last = j;
                    if (commonEdges > 1) { 
                        // In case of more than one instance of the vertex, it does the "cut"
                        vec[i].second = 0;  //The distance between them is set to 0, to represent the "cut"
                        c.second->deleteEdge(last); // Removes the first vertex instance
                        c.second->setAccess(true); // Changes the vertex acess entry/exit
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 5 - Partitions
void GPX2::findAllPartitions()
{    //goes through the graph finding all partition that there are in it
    int id{ 1 };
    vector<vector<string>> partitions;
    vector<string> cities;

    //gets the list of all cities
    for (auto node : unitedGraph) {cities.push_back(node.first);}

    while (!cities.empty()) { //while there still are cities in the list
        partitions.push_back(GPX2Support::findPartition(cities.front(),unitedGraph));// inserts the partition in the partition list

        for (string i : partitions.back()) {
            cities.erase(remove(cities.begin(), cities.end(), i), cities.end());
        }
    }

    for (vector<string> vi : partitions) { // removes partition with just one element
        if (vi.size() != 1) {
            vector<string> accessNodes;
            for (string idN : vi) {
                if (unitedGraph[idN]->getAccess()) {
                    accessNodes.push_back(idN);
                }
            }
            candidatePartitions.insert(make_pair(id, new Partition(id, vi, accessNodes)));
            id++;
        }
    }

    GPX2Support::cleanInsideAccess(candidatePartitions,unitedGraph);
}

// -----------------------------------------------------------------------------

// STEP 6 - Check Recombinant Partition

void GPX2::checkAllPartitions()
{
    GPX2Support::setAllEntryAndExits(candidatePartitions,red,blue);

    for(auto cpart : candidatePartitions){
        if(GPX2Support::checkPartition(cpart.second,red,blue)){
            feasiblePartitions.insert(make_pair(cpart.first,cpart.second));
        }else{
            unfeasiblePartitions.insert(make_pair(cpart.first,cpart.second));
        }
    }
    candidatePartitions.clear();
}

void GPX2::fusion()
{// Method to execute the GPX2's Fusion, it will check amongst the unfeasibe partitions and try to transform them to feasible ones
    bool atLeastOneConnected{ false };
    //Keps executing while there are still more than one partition to try to fuse them
    while ((unfeasiblePartitions.size() > 1)) {
        //Check how many partitions are connected, the fusion only happens if the partitions are connected by at least one node
        atLeastOneConnected = GPX2Fusion::unfeasiblePartitionsConnected(unfeasiblePartitions, unitedGraph);
        if (!atLeastOneConnected) {
            break;
        }
        // Counts how many times that one partition connects with other unfeasible partition, that is, the nodes that have a path that goes from one partition to another
        GPX2Fusion::countConnectedPartitions(unfeasiblePartitions);

        // Generates the fusion pairs that will be tried, this method already validates them
        GPX2Fusion::generateFusionPairs(fuseWith, unfeasiblePartitions);

        GPX2Fusion::fusePartitions(fuseWith,
                        unfeasiblePartitions,
                        unitedGraph,
                        red,
                        blue);

        for (auto part : unfeasiblePartitions) {
            part.second->getConnectedTo().clear();
            part.second->getConnections().clear();
        }

        //Checks if the generated partition is feasible, in that case it will be put in the feasible partition list
        GPX2Fusion::checkUnfeasiblePartitions(feasiblePartitions,
                                                unfeasiblePartitions,
                                                red,
                                                blue);
    }
}

// -----------------------------------------------------------------------------

// STEP 7 - Chosing the partition for the child

void GPX2::choose()
{
    //Find all exits and entries
    for (auto p : feasiblePartitions) {
        double totalRed{ 0 }, totalBlue{ 0 };
        for (auto pair : p.second->getEntryAndExits()) {
            totalRed += GPX2Support::partialDistance(pair.first, pair.second, red, p.second);
            totalBlue += GPX2Support::partialDistance(pair.first, pair.second, blue, p.second);
        }
        if (totalRed < totalBlue) {
            partitionsChoosen.push_back(GPX2Structs::Parent::RED);
        } else {
            partitionsChoosen.push_back(GPX2Structs::Parent::BLUE);
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 8 - Generates the map for the child

void GPX2::buildOffspring()
{   
    int index{ 0 };
    for (auto& allP : feasiblePartitions) {
        // if the blue is better than the red in that partition
        if (partitionsChoosen[index] == GPX2Structs::Parent::BLUE) { 
            for (string s : allP.second->getNodes()) {
                delete red[s];
                red.erase(s);

                Node* newNode = new Node(blue[s]->getId());
                newNode->setEdges(blue[s]->getEdges());
                red.insert(make_pair(s, newNode));
            }
        } else {
            for (string s : allP.second->getNodes()) {
                delete blue[s];
                blue.erase(s);

                Node* newNode = new Node(red[s]->getId());
                newNode->setEdges(red[s]->getEdges());
                blue.insert(make_pair(s, newNode));
            }
        }
        index++;
    }

    offspringChoosen = ((GPX2Support::totalDistance(red) < GPX2Support::totalDistance(blue)) ? (GPX2Structs::Parent::RED) : (GPX2Structs::Parent::BLUE));
}

// -----------------------------------------------------------------------------
// STEP 9 - Linearize the offspring's map and remove ghosts
vector<int> GPX2::convertChoosenChild(){
    if (offspringChoosen == GPX2Structs::Parent::RED) {
        GPX2Support::removeGhosts(red);
        return(GPX2Support::mapToTour(red));
    } else {
        GPX2Support::removeGhosts(blue);
        return(GPX2Support::mapToTour(blue));
    }
}

GPX2::GPX2() {}

GPX2::~GPX2()
{
    GPX2Support::deleteNodeMap(red);
    GPX2Support::deleteNodeMap(blue);
    GPX2Support::deleteNodeMap(unitedGraph);
    GPX2Support::deletePartitionMap(candidatePartitions);
    GPX2Support::deletePartitionMap(feasiblePartitions);
    GPX2Support::deletePartitionMap(unfeasiblePartitions);
    partitionsChoosen.clear();
    feasiblePartitions.clear();
}