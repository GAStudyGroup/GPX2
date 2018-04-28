#include "GPX2Support.hpp"
#include "Utils.hpp"
#include "Globals.hpp"

#include <deque>
using std::deque;

#include <algorithm>
using std::find;

#include <iostream>

#include <iterator>
using std::distance;

//From string
using std::to_string;

//from utility
using std::make_pair;

bool GPX2Support::checkPartition(Partition* partition, GPX2Structs::NodeMap &red, GPX2Structs::NodeMap &blue)
{
    unsigned size = partition->getAccessNodes().size();
    vector<string> redNodes, blueNodes;
    redNodes = blueNodes = partition->getNodes();
    if (!(size % 2 == 0)) {
        //It's not feasible because the number of entries and exits it's different
        return (false);
    } else {
        vector<string> nodesInPartition = partition->getNodes();

        //Don't have the same entries an exits in both parents
        if (partition->getEntryAndExits().empty()) {
            return (false);
        } else {
            for (pair<string, string> nodes : partition->getEntryAndExits()) {
                pair<GPX2Structs::SearchResult, vector<string>> resultRed = DFS_inside(nodes.first, nodes.second, red, partition);
                pair<GPX2Structs::SearchResult, vector<string>> resultBlue = DFS_inside(nodes.first, nodes.second, blue, partition);
                eraseSubVector(redNodes, resultRed.second);
                eraseSubVector(blueNodes, resultBlue.second);
            }
            if (redNodes.empty() && blueNodes.empty()) {
                return (true);
            } else {
                return (false);
            }
        }
    }
}

void GPX2Support::cleanInsideAccess(GPX2Structs::PartitionMap &partitions, GPX2Structs::NodeMap &unitedGraph)
{//Removes de Acess flags from the vertices that connects the partition to itself

    //Goes through the partition
    for (auto& p : partitions) {
        //accessNodesAV - already visited
        vector<string> testedAccessNodes,accessNodesAV;

        vector<string> accessNodes = p.second->getAccessNodes();

        for (string node : accessNodes) {
            //If the node doenst have been visited already
            if(find(accessNodesAV.begin(),accessNodesAV.end(),node)==accessNodesAV.end()){
                //check if node is connected to another partition
                pair<GPX2Structs::SearchResult, vector<string>> result = DFS_outside(node, partitions, unitedGraph);

                if (result.first == GPX2Structs::SearchResult::CONNECTED_TO_PARTITION) {
                    testedAccessNodes.push_back(node);
                    accessNodesAV.push_back(node);
                } else {
                    //put the distance value in the edges of the nodes that are inserted in the partition 
                    for(string node : result.second){
                        for(auto &edge : unitedGraph[node]->getEdges()){
                            edge.second = distance(node, edge.first);
                        }
                    }

                    accessNodesAV.push_back(result.second.front());
                    accessNodesAV.push_back(result.second.back());

                    result.second.erase(result.second.begin());
                    result.second.erase(result.second.end());
                    
                    

                    p.second->getNodes().insert(p.second->getNodes().end(),result.second.begin(), result.second.end());
                }
            }
        }
        p.second->setAccessNodes(testedAccessNodes);
    }
}

bool GPX2Support::comparePairString(const pair<string, string>& p1, const pair<string, string>& p2)
{
    bool first = (!(p1.first.compare(p2.first)) || !(p1.first.compare(p2.second)));
    bool second = (!(p1.second.compare(p2.first)) || !(p1.second.compare(p2.second)));
    return ((first && second));
}

void GPX2Support::deletePartitionMap(GPX2Structs::PartitionMap& m)
{
    // deallocate the pointers and set them to null, then clear the map

    for (auto& it : m) {
        delete it.second;
        it.second = nullptr;
    }
    m.clear();
}

void GPX2Support::deleteNodeMap(GPX2Structs::NodeMap& m)
{ // deletar o mapa completamente,
    // desalocando os ponteiros tb
    for (auto& it : m) {
        delete it.second;
        it.second = nullptr;
    }
    m.clear();
}

pair<GPX2Structs::SearchResult, vector<string>> GPX2Support::DFS_inside(string entry, string exit, GPX2Structs::NodeMap father, Partition* partitionPtr)
{
    //DFS inside the partition
    string now;
    vector<string> alreadyVisited;
    deque<string> nextToVisit;
    vector<string> partition = partitionPtr->getNodes();
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };
    bool isInPartition{ false };

    nextToVisit.push_back(entry);

    while (!nextToVisit.empty()) {

        now = nextToVisit.back();
        nextToVisit.pop_back();
        alreadyVisited.push_back(now);
        vector<Node::node> edges = father[now]->getEdges();

        for (Node::node cn : edges) {

            notAlreadyVisited = (find(alreadyVisited.begin(), alreadyVisited.end(), cn.first) == alreadyVisited.end());
            notToVisit = (find(nextToVisit.begin(), nextToVisit.end(), cn.first) == nextToVisit.end());
            isInPartition = (find(partition.begin(), partition.end(), cn.first) != partition.end());

            if (notAlreadyVisited && notToVisit && isInPartition) {
                nextToVisit.push_back(cn.first);
            }
        }
    }
    return (make_pair((alreadyVisited.back() == exit ? GPX2Structs::SearchResult::IS_CONNECTED : GPX2Structs::SearchResult::IS_NOT_CONNECTED), alreadyVisited));
}

pair<GPX2Structs::SearchResult, vector<string>> GPX2Support::DFS_outside(string id, GPX2Structs::PartitionMap &partitions, GPX2Structs::NodeMap &unitedGraph)
{
    string now;
    int idPartition = whichPartition(id, partitions);
    vector<string> partition = partitions[idPartition]->getNodes();
    vector<string> alreadyVisited;
    deque<string> nextToVisit;
    int partitionConnected{ -1 };
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };
    bool cutEdge{ false };

    nextToVisit.push_back(id);
    
    while (!nextToVisit.empty()) {
        now = nextToVisit.back();
        nextToVisit.pop_back();

        alreadyVisited.push_back(now);

        bool endNode{false};

        vector<Node::node> edges = unitedGraph[now]->getEdges();
        for (Node::node cn : edges) {
            notAlreadyVisited = (find(alreadyVisited.begin(), alreadyVisited.end(), cn.first) == alreadyVisited.end());
            notToVisit = (find(nextToVisit.begin(), nextToVisit.end(), cn.first) == nextToVisit.end());
            cutEdge = (cn.second == 0);
            if (notAlreadyVisited && notToVisit && cutEdge) {
                nextToVisit.push_back(cn.first);
            }
            //if the node has a edge that was not cut, it must be a endNode
            if(!cutEdge){
                endNode = true;
            }
        }

        if(endNode){
            partitionConnected = whichPartition(now, partitions);
            if(idPartition != partitionConnected){
                return make_pair(GPX2Structs::SearchResult::CONNECTED_TO_PARTITION, alreadyVisited);
            }
        }
    }
    
    unitedGraph[id]->setAccess(false);
    unitedGraph[alreadyVisited.back()]->setAccess(false);
    return make_pair(GPX2Structs::SearchResult::CONNECTED_TO_SELF, alreadyVisited);
}

void GPX2Support::eraseSubVector(vector<string>& vec, vector<string>& subvec)
{
    for (unsigned i = 0; i < subvec.size(); i++) {
        for (unsigned j = 0; j < vec.size(); j++) {
            if (subvec[i].compare(vec[j]) == 0) {
                vec.erase(vec.begin() + j);
                break;
            }
        }
    }
}

vector<string> GPX2Support::findPartition(const string nodeOne, GPX2Structs::NodeMap &unitedGraph)
{ //Given a value, this method will use nodeOne as a root which will be the first point of the search in Gu', when it finds the partition in which the node is contained the method will return a vector with the ids of the citites that compose the partition

    Node* root; // node to be verified
    vector<string> idAlreadyVisited; // visited nodes list
    deque<string> nextToVisit; // list of the next nodes to visit
    vector<string> partition;

    bool notAlreadyVisited{ false };
    bool notToVisit{ false };
    bool notCut{ false };

    nextToVisit.push_back(nodeOne); // first node to be visited

    while (!nextToVisit.empty()) { // while there are nodes to visit
        root = unitedGraph[nextToVisit.front()]; // node in which the BFS will start
        nextToVisit.pop_front(); // Removes the node from the list
        partition.push_back(root->getId()); // inserts the node in the partition
        idAlreadyVisited.push_back(root->getId()); // Nodes in the "Already visited" list

        for (Node::node n : root->getEdges()) { //goes through the connected edges
            notAlreadyVisited = find(idAlreadyVisited.begin(), idAlreadyVisited.end(), n.first) == idAlreadyVisited.end(); // checks if the edge was visited
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end(); //checks if it isn't in the list to be visited
            notCut = n.second != 0; // checks if the edge was "cut"
            if (notAlreadyVisited && notToVisit && notCut) { // case if all conditions are satisfied
                nextToVisit.push_back(
                    n.first); // puts in the list of the next to be visited
            }
        }
    }
    return (partition); // returns a vector with the partition
}

vector<pair<string, string>> GPX2Support::getEntryAndExitList(Partition* p, GPX2Structs::NodeMap &red, GPX2Structs::NodeMap &blue)
{
    vector<pair<string, string>> entryAndExit;
    vector<string> accessVec = p->getAccessNodes();
    for (unsigned i = 0; i < accessVec.size(); i++) {
        for (unsigned j = 0; j < accessVec.size(); j++) {
            if (i != j) {
                pair<GPX2Structs::SearchResult, vector<string>> resultRed = GPX2Support::DFS_inside(accessVec[i], accessVec[j], red, p);
                pair<GPX2Structs::SearchResult, vector<string>> resultBlue = GPX2Support::DFS_inside(accessVec[i], accessVec[j], blue, p);
                if (resultRed.first == GPX2Structs::SearchResult::IS_CONNECTED && resultBlue.first == GPX2Structs::SearchResult::IS_CONNECTED) {
                    entryAndExit.push_back(make_pair(accessVec[i], accessVec[j]));
                }
            }
        }
    }

    for (auto itOut = entryAndExit.begin(); itOut != entryAndExit.end(); itOut++) {
        for (auto itIn = entryAndExit.begin(); itIn != entryAndExit.end();) {
            if (itOut != itIn) {
                if (comparePairString(*itOut, *itIn)) {
                    itIn = entryAndExit.erase(itIn);
                } else {
                    itIn++;
                }
            } else {
                itIn++;
            }
        }
    }
    return (entryAndExit);
}

void GPX2Support::insertGhost(string& id, GPX2Structs::NodeMap& tour, Node* ghost)
{
    Node::node edgeFirst = tour[id]->getEdges()[0]; // edge to be added to the ghostNode
    tour[id]->deleteEdge(0); // delete edge
    tour[id]->addEdge(Node::node(ghost->getId(), 0)); // Adds to the original node the linking edge with the ghost

    for (unsigned i = 0; i < 2; i++) {
        // goes by the linking edges with the node connected with the ghost
        Node::node edge = tour[edgeFirst.first]->getEdges()[i];

        if (edge.first.compare(id) == 0) { // checks if it is the connection

            //adds the connection to the ghost and removes it from the original node
            tour[edgeFirst.first]->deleteEdge(i);
            tour[edgeFirst.first]->addEdge(Node::node(ghost->getId(), edgeFirst.second));
        }
    }

    ghost->addEdge(edgeFirst); // connects the ghost to the edge
    ghost->setAccess(true);
    ghost->addEdge(Node::node(tour[id]->getId(), 0)); // adds the original node as and edge of the ghost

    tour.insert(make_pair(ghost->getId(), ghost)); // inserts the ghost in the parent map
}

vector<int> GPX2Support::mapToTour(GPX2Structs::NodeMap& mapOffspring)
{
    vector<int> offspring;
    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;

    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    Node* city = mapOffspring.begin()->second; 

    isAlreadyVisited.push_back(mapOffspring.begin()->first);

    offspring.push_back(stoi(city->getId())); // begin the linearization by the first node of the map and inserts it in the offspring

    nextToVisit.push_back(city->getEdges()[0].first);

    while (!nextToVisit.empty()) {

        isAlreadyVisited.push_back(nextToVisit.front());
        city = mapOffspring[nextToVisit.front()];
        nextToVisit.pop_front();
        // criates a city object and inserts it in the tour
        offspring.push_back(stoi(city->getId()));

        for (Node::node n : city->getEdges()) {

            notAlreadyVisited = find(isAlreadyVisited.begin(), isAlreadyVisited.end(), n.first) == isAlreadyVisited.end();
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end();

            if (notAlreadyVisited && notToVisit) {
                nextToVisit.push_back(n.first);
            }
        } 
    }

    return (offspring);
}

int GPX2Support::partialDistance(string entry, string exit, GPX2Structs::NodeMap father, Partition* partitionPtr)
{ // Distancia do SubTour da partição no pai
    //fazer uma busca em profundidade dentro da partição
    string now;
    vector<string> alreadyVisited;
    deque<string> nextToVisit;
    vector<string> partition = partitionPtr->getNodes();
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };
    bool isInPartition{ false };

    int totalDistance{ 0 };

    nextToVisit.push_back(entry);

    while (!nextToVisit.empty()) {

        now = nextToVisit.back();
        nextToVisit.pop_back();
        alreadyVisited.push_back(now);
        vector<Node::node> edges = father[now]->getEdges();

        for (Node::node cn : edges) {

            notAlreadyVisited = (find(alreadyVisited.begin(), alreadyVisited.end(), cn.first) == alreadyVisited.end());
            notToVisit = (find(nextToVisit.begin(), nextToVisit.end(), cn.first) == nextToVisit.end());
            isInPartition = (find(partition.begin(), partition.end(), cn.first) != partition.end());

            if (notAlreadyVisited && notToVisit && isInPartition) {
                totalDistance += cn.second;
                nextToVisit.push_back(cn.first);
            }
        }
    }
    return (totalDistance);
}

void GPX2Support::printMap(GPX2Structs::NodeMap& graph, std::ostream& stream)
{
    stream << "size: " << graph.size() << "\n";
    for (map<string, Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        stream << " " << it->first << " | " << graph[it->first]->getId() <<  "\n";
        if (it->second->getAccess()) {
            stream << "Access: " << true <<  "\n";
        } else {
            stream << "Access: " << false <<  "\n";
        }
        stream << "==================================\n";

        stream << "connections: " << it->second->getEdges().size() <<  "\n";
        for (unsigned i = 0; i < it->second->getEdges().size(); i++) {
            stream << "Edge " << i << ": " << graph[it->second->getEdges()[i].first]->getId();
            stream << "\t#\t";
            stream << "Distance: " << it->second->getEdges()[i].second << "\n";
        }
        stream << "----------------------------------\n";
    }

    stream << "______________________________________________________________THE DIVISION______________________________________________________________\n";

    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    Node* city = graph.begin()->second; // Reduzir chamadas (Node dentro do map)

    isAlreadyVisited.push_back(graph.begin()->first);
    stream << graph.begin()->first << "\n";
    nextToVisit.push_back(city->getEdges()[0].first);

    while (!nextToVisit.empty()) {

        stream << nextToVisit.front() << "\n";
        isAlreadyVisited.push_back(nextToVisit.front());
        city = graph[nextToVisit.front()];
        nextToVisit.pop_front();

        for (Node::node n : city->getEdges()) {

            notAlreadyVisited = find(isAlreadyVisited.begin(), isAlreadyVisited.end(), n.first) == isAlreadyVisited.end();
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end();

            if (notAlreadyVisited && notToVisit) {
                nextToVisit.push_back(n.first);
            }
        }
    }
    stream << "size " << isAlreadyVisited.size() << "\n";
    stream << "map size " << graph.size() << "\n";
    stream.flush();
}

void GPX2Support::removeGhosts(GPX2Structs::NodeMap& graph)
{
    for (auto it = graph.begin();it!=graph.end();) {
        auto index = (*it).first.find(GPX2Structs::ghostToken);
        if (index != string::npos) {
            // gets the id of the node without the ghost token
            string id = (*it).first;
            id.erase(index, GPX2Structs::ghostToken.size());

            //Gets two nodes that are connected to the original node, and also to the ghost, iterating through the edges of the original node
            vector<Node::node> edges = graph[id]->getEdges();
            unsigned edgeToDelete{ 0 };
            Node::node prev, next;
            for (unsigned i = 0; i < edges.size(); i++) {
                //If it found the edge that is connected to the ghost...
                if (!edges[i].first.compare((*it).first)) {
                    edgeToDelete = i;
                } else {
                    prev = edges[i];
                }
            }
            graph[id]->deleteEdge(edgeToDelete);

            edges = graph[(*it).first]->getEdges();

            for (unsigned i = 0; i < edges.size(); i++) {
                //Find the nodes that aren't the original in the edge list 
                if (!(!edges[i].first.compare(id))) {
                    next = edges[i];
                    break;
                }
            }

            //Connect the original node to the next
            graph[id]->addEdge(next);

            //arrange the next node edges
            edges = graph[next.first]->getEdges();
            for (unsigned i = 0; i < edges.size(); i++) {
                //Found the edge that references the ghost
                if (!edges[i].first.compare((*it).first)) {
                    edgeToDelete = i;
                    break;
                }
            }
            graph[next.first]->deleteEdge(edgeToDelete);
            graph[next.first]->addEdge(make_pair(id, next.second));

            //removes the ghost node
            delete graph[(*it).first];
            it = graph.erase(it);
        }else{
            it++;
        }
    }
}

void GPX2Support::setAllEntryAndExits(GPX2Structs::PartitionMap &partitions, GPX2Structs::NodeMap &red, GPX2Structs::NodeMap &blue)
{
    for (auto p : partitions) {
        p.second->setEntryAndExits(getEntryAndExitList(p.second,red,blue));
    }
}

int GPX2Support::totalDistance(GPX2Structs::NodeMap& graph) 
{ // Distancia total para percorrer o grafo
    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    int totalDistance{ 0 };
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    Node *city = graph.begin()->second, *last = nullptr; // Reduzir chamadas (Node dentro do map)
    isAlreadyVisited.push_back(graph.begin()->first);

    nextToVisit.push_back(city->getEdges()[0].first);
    totalDistance += city->getEdges()[0].second;

    while (!nextToVisit.empty()) {
        isAlreadyVisited.push_back(nextToVisit.front());
        city = graph[nextToVisit.front()];
        nextToVisit.pop_front();

        for (Node::node n : city->getEdges()) {

            notAlreadyVisited = find(isAlreadyVisited.begin(), isAlreadyVisited.end(), n.first) == isAlreadyVisited.end();
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end();

            if (notAlreadyVisited && notToVisit) {
                nextToVisit.push_back(n.first);
                totalDistance += n.second;
            }
        }
        last = city;
    }
    for (Node::node n : last->getEdges()) {
        if (n.first.compare(graph.begin()->first) == 0) {
            totalDistance += n.second;
        }
    }
    return totalDistance;
}

GPX2Structs::NodeMap GPX2Support::tourToMap(vector<int> &t)
{
    //Maps the tour to a graph with connections between the nodes
    std::map<string, Node*> aux; 
    double dist = 0;
    vector<int> cities{ t };
    unsigned size = t.size();

    for (unsigned i = 0; i < size; i++) {
        Node* cn = new Node(to_string(cities[i]));
        if(i==0){
            //if the current node is the first one, connect it with the last one and the next
            //Node::node is a pair of string and double, the ID and distance
            dist = distance(cities[size-1],cities[i]);
            cn->getEdges().push_back(Node::node(to_string(cities[size-1]), dist));
            dist = distance(cities[i],cities[i+1]);
            cn->getEdges().push_back(Node::node(to_string(cities[i+1]), dist));
        }else if(i==(size-1)){
            //if the current node is the last one, connect it with the previous and the first node
            dist = distance(cities[i-1],cities[i]);
            cn->getEdges().push_back(Node::node(to_string(cities[i-1]), dist));
            dist = distance(cities[i],cities[0]);
            cn->getEdges().push_back(Node::node(to_string(cities[0]), dist));
        }else{
            //for all the other nodes connect if with the previous and next
            dist = distance(cities[i-1],cities[i]);
            cn->getEdges().push_back(Node::node(to_string(cities[i-1]), dist));
            dist = distance(cities[i],cities[i+1]);
            cn->getEdges().push_back(Node::node(to_string(cities[i+1]), dist));
        }

        aux.insert(make_pair(cn->getId(), cn));
    }

    return (aux); //returns a map with the nodes already instantiated and added 
}


int GPX2Support::whichPartition(const string id, GPX2Structs::PartitionMap partitions)
{ // Procura em qual partição está a cidade procurada, retorna o ID da partição
    for (auto& p : partitions) {
        if (find(p.second->getNodes().begin(), p.second->getNodes().end(), id) != p.second->getNodes().end()) {
            return (p.first);
        }
    }
    return (-1);
}

void printMapAndPartitions(std::ostream& out, vector<int> offspring, GPX2Structs::PartitionMap partitions, vector<GPX2Structs::Parent> partitionsChoosen, GPX2Structs::PartitionMap feasible) {
    int inPartition{-1};
    bool beforePartition{false};

    for(auto p : partitionsChoosen) {
        std::cout << ((p == GPX2Structs::Parent::RED)?"RED":"BLUE") << std::endl; 
    }

    out << "//#color #000000FF\n";
    out << "//#fitness " << getFitness(offspring) << "\n";

    for(auto p : partitions) {
        std::cout << *p.second << std::endl;
    }

    for(int node : offspring) {
        int partition{GPX2Support::whichPartition(std::to_string(node), partitions)};

        // Not in partition
        if(partition == -1) {
            if(beforePartition) {
                out << "\n";
                City city{Globals::map.getCityById(node)};
                out << city.getId() << " " << city.getX() << " " << city.getY() << "\n";
                beforePartition = false;
            } else {
                City city{Globals::map.getCityById(node)};
                out << city.getId() << " " << city.getX() << " " << city.getY() << "\n";
            }
        } else {
            if(partition == inPartition) {
                City city{Globals::map.getCityById(node)};
                out << city.getId() << " " << city.getX() << " " << city.getY() << "\n";
            } else {
                beforePartition = true;
                inPartition = partition;
                out << "\n";
                


                std::cout << ((partitionsChoosen[partition] == GPX2Structs::Parent::RED)?" RED":" BLUE") << std::endl; 
                out << (partitionsChoosen[distance(feasible.begin(),feasible.find(partition))]==GPX2Structs::Parent::RED?"#FF0000FF":"#0000FFFF") << "\n"; 

                City city{Globals::map.getCityById(node)};
                out << city.getId() << " " << city.getX() << " " << city.getY() << "\n";
            }
        }
    }
    out << std::endl;
}

























