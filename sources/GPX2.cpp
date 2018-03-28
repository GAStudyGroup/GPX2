#include "GPX2.hpp"
#include "Config.hpp"

Tour GPX2::crossover(Tour redT, Tour blueT)
{
    GPX2 obj;
    // Step 1
    obj.red = obj.tourToMap(redT);
    obj.blue = obj.tourToMap(blueT);

    // Step 2
    obj.createGhosts();

    // Step 3
    obj.joinGraphs();

    // Step 4
    obj.cutCommonEdges();

    // Step 5
    obj.findAllPartitions();
    obj.cleanInsideAccess();

    // If there are less than 1 partition the GPX can't recombine them
    if (obj.feasiblePartitions.size() < 2) {
        return ((redT.getFitness() < blueT.getFitness()) ? redT : blueT);
    }

    // Step 6
    obj.setAllEntryAndExits();
    obj.checkAllPartitions();

    // Fusion
    //It's needed to have more than one unfeasible partition to fuse them
    if (obj.unfeasiblePartitions.size() > 1) {
        obj.fusion();
    }

    if (obj.feasiblePartitions.size() < 2) {
        return ((redT.getFitness() < blueT.getFitness()) ? redT : blueT);
    }

    // Step 7
    obj.choose();

    // Step 8
    obj.buildOffspring();
    Tour t;
    if (obj.offspringChoosen == Parent::RED) {
        obj.removeGhosts(obj.red);
        // Step 9
        t = obj.mapToTour(obj.red);
    } else {
        obj.removeGhosts(obj.blue);
        // Step 9
        t = obj.mapToTour(obj.blue);
    }

    return t;
}

// -----------------------------------------------------------------------------

// STEP 1 - Tour Mapping
GPX2::CityMap GPX2::tourToMap(Tour& t)
{
    if (t.getRoute().empty()) {
        exit(EXIT_FAILURE);
    }
    //Maps the tour to a graph with connections between the nodes
    std::map<string, CityNode*> aux; 
    double dist = 0;
    vector<int> cities{ t.getRoute() };
    unsigned size = t.getRoute().size();

    for (unsigned i = 0; i < size; i++) {
        CityNode* cn = new CityNode(to_string(cities[i]));
        if(i==0){
            //if the current node is the first one, connect it with the last one and the next
            dist = distance(cities[size-1],cities[i]);
            cn->getEdges().push_back(CityNode::node(to_string(cities[size-1]), dist));
            dist = distance(cities[i],cities[i+1]);
            cn->getEdges().push_back(CityNode::node(to_string(cities[i+1]), dist));
        }else if(i==(size-1)){
            //if the current node is the last one, connect it with the previous and the first node
            dist = distance(cities[i-1],cities[i]);
            cn->getEdges().push_back(CityNode::node(to_string(cities[i-1]), dist));
            dist = distance(cities[i],cities[0]);
            cn->getEdges().push_back(CityNode::node(to_string(cities[0]), dist));
        }else{
            //for all the other nodes connect if with the previous and next
            dist = distance(cities[i-1],cities[i]);
            cn->getEdges().push_back(CityNode::node(to_string(cities[i-1]), dist));
            dist = distance(cities[i],cities[i+1]);
            cn->getEdges().push_back(CityNode::node(to_string(cities[i+1]), dist));
        }

        aux.insert(make_pair(cn->getId(), cn));
    }

    return (aux); //returns a map with the nodes already instantiated and added 
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
        string ghostID = id+ghostToken;

        CityNode* ghostNode = new CityNode(ghostID);
        insertGhost(id, red, ghostNode);

        ghostNode = new CityNode(ghostID);
        insertGhost(id, blue, ghostNode);
    }
}

void GPX2::insertGhost(string& id, CityMap& tour, CityNode* ghost)
{
    CityNode::node edgeFirst = tour[id]->getEdges()[0]; // edge to be added to the ghostNode
    tour[id]->deleteEdge(0); // delete edge
    tour[id]->addEdge(CityNode::node(ghost->getId(), 0)); // Adds to the original node the linking edge with the ghost

    for (unsigned i = 0; i < 2; i++) {
        // goes by the linking edges with the node connected with the ghost
        CityNode::node edge = tour[edgeFirst.first]->getEdges()[i];

        if (edge.first.compare(id) == 0) { // checks if it is the connection

            //adds the connection to the ghost and removes it from the original node
            tour[edgeFirst.first]->deleteEdge(i);
            tour[edgeFirst.first]->addEdge(CityNode::node(ghost->getId(), edgeFirst.second));
        }
    }

    ghost->addEdge(edgeFirst); // connects the ghost to the edge
    ghost->setAccess(true);
    ghost->addEdge(CityNode::node(tour[id]->getId(), 0)); // adds the original node as and edge of the ghost

    tour.insert(make_pair(ghost->getId(), ghost)); // inserts the ghost in the parent map
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
        CityNode* c = red[id];
        // creates the entry in the union map
        unitedGraph.insert(
            make_pair((c->getId()), new CityNode((c->getId()))));
        // puts the edges in the union/*  */ map
        for (CityNode::node n : red[(c->getId())]->getEdges()) {
            unitedGraph[(c->getId())]->addEdge(make_pair(n.first, n.second));
        }
        for (CityNode::node n : blue[(c->getId())]->getEdges()) {
            unitedGraph[(c->getId())]->addEdge(make_pair(n.first, n.second));
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 4 - Creating the GU' (cutting the common edges)

void GPX2::cutCommonEdges()
{ // executes the process of cutting the common edges between the parents from the union graph
    for (auto &c : unitedGraph) { // goes by all entres from Gu
        vector<CityNode::node>& vec = c.second->getEdges(); //Loads the vector with the entries contained in the map entry
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

vector<string> GPX2::findPartition(const string nodeOne)
{ //Given a value, this method will use nodeOne as a root which will be the first point of the search in Gu', when it finds the partition in which the node is contained the method will return a vector with the ids of the citites that compose the partition

    CityNode* root; // node to be verified
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

        for (CityNode::node n : root->getEdges()) { //goes through the connected edges
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

void GPX2::findAllPartitions()
{    //goes through the graph finding all partition that there are in it
    int id{ 1 };
    vector<vector<string>> partitions;
    vector<string> cities;

    //gets the list of all cities
    for (auto node : unitedGraph) {cities.push_back(node.first);}

    while (!cities.empty()) { //while there still are cities in the list
        partitions.push_back(findPartition(cities.front()));// inserts the partition in the partition list

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
            feasiblePartitions.insert(make_pair(id, new Partition(id, vi, accessNodes)));
            id++;
        }
    }
}

/* 
    Future BUG?
 */

void GPX2::cleanInsideAccess()
{//Removes de Acess flags from the vertices that connects the partition to itself

    //Goes through the partition
    for (auto& p : feasiblePartitions) {
        vector<string> tmp;

        vector<string> accessNodes = p.second->getAccessNodes();
        for (auto it = accessNodes.begin(); it != accessNodes.end(); it++) {
            //Checks if the vertex is connected to other edges
            pair<SearchResult, vector<string>> result = DFS_outside((*it), feasiblePartitions);

            if (result.first == SearchResult::CONNECTED_TO_PARTITION) {
                tmp.push_back((*it));
            } else {
                accessNodes.erase(remove(accessNodes.begin(), accessNodes.end(), result.second.back()), accessNodes.end());
                if (!result.second.empty()) {
                    result.second.erase(result.second.begin());
                }
                if (!result.second.empty()) {
                    result.second.erase(result.second.end());
                }
                // Inserts the connection path in the partition
                p.second->getNodes().insert(p.second->getNodes().end(), result.second.begin(), result.second.end());
            }
        }
        p.second->setAccessNodes(tmp);
    }
}

// -----------------------------------------------------------------------------

// STEP 6 - Check Recombinant Partition

void GPX2::checkAllPartitions()
{
    for (auto it = feasiblePartitions.begin(); it != feasiblePartitions.end();) {
        //If it's not a recombinant partition, remove from the partition list
        if (!checkPartition((*it).second)) {
            unfeasiblePartitions.insert(make_pair((*it).first, (*it).second));
            it = feasiblePartitions.erase(it);
        } else {
            it++;
        }
    }
}

bool GPX2::checkPartition(Partition* partition)
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
                pair<SearchResult, vector<string>> resultRed = DFS_inside(nodes.first, nodes.second, red, partition);
                pair<SearchResult, vector<string>> resultBlue = DFS_inside(nodes.first, nodes.second, blue, partition);
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

void GPX2::fusion()
{// Method to execute the GPX2's Fusion, it will check amongst the unfeasibe partitions and try to transform them to feasible ones
    bool atLeastOneConnected{ false };
    //Keps executing while there are still more than one partition to try to fuse them
    while ((unfeasiblePartitions.size() > 1)) {
        //Check how many partitions are connected, the fusion only happens if the partitions are connected by at least one node
        atLeastOneConnected = unfeasiblePartitionsConnected();
        if (!atLeastOneConnected) {
            break;
        }
        // Counts how many times that one partition connects with other unfeasible partition, that is, the nodes that have a path that goes from one partition to another
        countConnectedPartitions();

        // Generates the fusion pairs that will be tried, this method already validates them
        generateFusionPairs();

        fusePartitions();

        for (auto part : unfeasiblePartitions) {
            part.second->getConnectedTo().clear();
            part.second->getConnections().clear();
        }

        //Checks if the generated partition is feasible, in that case it will be put in the feasible partition list
        checkUnfeasiblePartitions();
    }
}

void GPX2::setAllEntryAndExits()
{
    for (auto p : feasiblePartitions) {
        p.second->setEntryAndExits(getEntryAndExitList(p.second));
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
            totalRed += partialDistance(pair.first, pair.second, red, p.second);
            totalBlue += partialDistance(pair.first, pair.second, blue, p.second);
        }
        if (totalRed < totalBlue) {
            partitionsChoosen.push_back(Parent::RED);
        } else {
            partitionsChoosen.push_back(Parent::BLUE);
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
        if (partitionsChoosen[index] == Parent::BLUE) { 
            for (string s : allP.second->getNodes()) {
                delete red[s];
                red.erase(s);

                CityNode* newNode = new CityNode(blue[s]->getId());
                newNode->setEdges(blue[s]->getEdges());
                red.insert(make_pair(s, newNode));
            }
        } else {
            for (string s : allP.second->getNodes()) {
                delete blue[s];
                blue.erase(s);

                CityNode* newNode = new CityNode(red[s]->getId());
                newNode->setEdges(red[s]->getEdges());
                blue.insert(make_pair(s, newNode));
            }
        }
        index++;
    }

    offspringChoosen = ((totalDistance(red) < totalDistance(blue)) ? (Parent::RED) : (Parent::BLUE));
}

void GPX2::removeGhosts(CityMap& graph)
{
    for (CityMap::iterator it = graph.begin();it!=graph.end();) {
        size_t index = (*it).first.find(ghostToken);
        if (index != string::npos) {
            // gets the id of the node without the ghost token
            string id = (*it).first;
            id.erase(index, ghostToken.size());

            //Gets two nodes that are connected to the original node, and also to the ghost, iterating through the edges of the original node
            vector<CityNode::node> edges = graph[id]->getEdges();
            unsigned edgeToDelete{ 0 };
            CityNode::node prev, next;
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

// -----------------------------------------------------------------------------

// STEP 9 - Linearizar o mapa do filho
Tour GPX2::mapToTour(CityMap& mapOffspring)
{ // Map para tour
    Tour offspring;
    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;

    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    CityNode* city = mapOffspring.begin()->second; // Reduzir chamadas (CityNode dentro do map)

    isAlreadyVisited.push_back(mapOffspring.begin()->first);

    offspring.getRoute().push_back(stoi(city->getId())); // já foi visitado então entra no filho

    nextToVisit.push_back(city->getEdges()[0].first);

    while (!nextToVisit.empty()) {

        isAlreadyVisited.push_back(nextToVisit.front());
        city = mapOffspring[nextToVisit.front()];
        nextToVisit.pop_front();
        // Cria um objeto e carrega no Tour filho
        offspring.getRoute().push_back(stoi(city->getId()));

        for (CityNode::node n : city->getEdges()) {

            notAlreadyVisited = find(isAlreadyVisited.begin(), isAlreadyVisited.end(), n.first) == isAlreadyVisited.end();
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end();

            if (notAlreadyVisited && notToVisit) {
                nextToVisit.push_back(n.first);
            }
        } 
    }

    return (offspring);
}

/* 
    UTILITIES FUNCTIONS

    Funções de utilidade para ajudar o desenvolvimento dos STEPs

============================================================================
*/
bool GPX2::comparePairInt(const UnfeasibleConnection& p1, const UnfeasibleConnection& p2)
{
    bool first = p1.partitionId1 == p2.partitionId1 || p1.partitionId1 == p2.partitionId2;
    bool second = p1.partitionId2 == p2.partitionId1 || p1.partitionId2 == p2.partitionId2;

    return ((first && second));
}

bool GPX2::comparePairString(const pair<string, string>& p1, const pair<string, string>& p2)
{
    bool first = (!(p1.first.compare(p2.first)) || !(p1.first.compare(p2.second)));
    bool second = (!(p1.second.compare(p2.first)) || !(p1.second.compare(p2.second)));
    return ((first && second));
}

void GPX2::deletePartitionMap(PartitionMap& m)
{ // deletar o mapa completamente,
    // desalocando os ponteiros tb

    for (auto& it : m) {
        delete it.second;
        it.second = nullptr;
    }
    m.clear();
}

pair<GPX2::SearchResult, vector<string>> GPX2::DFS_outside(string id, PartitionMap partitions, bool unfeasible)
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

        vector<CityNode::node> edges = unitedGraph[now]->getEdges();

        for (CityNode::node cn : edges) {
            notAlreadyVisited = (find(alreadyVisited.begin(), alreadyVisited.end(), cn.first) == alreadyVisited.end());
            notToVisit = (find(nextToVisit.begin(), nextToVisit.end(), cn.first) == nextToVisit.end());
            cutEdge = cn.second == 0;

            if (notAlreadyVisited && notToVisit && cutEdge) {
                nextToVisit.push_back(cn.first);
            }
        }
    }

    //pegar o id da partition do nó que passou por último
    partitionConnected = whichPartition(alreadyVisited.back(), partitions);

    if (idPartition == partitionConnected) {
        //se conectar em si mesmo, seta o nó de entrada e o ultimo onde chegou como não access
        unitedGraph[id]->setAccess(false);
        unitedGraph[alreadyVisited.back()]->setAccess(false);
        return make_pair(SearchResult::CONNECTED_TO_SELF, alreadyVisited);
    } else {
        if (unfeasible) {
            partitions[idPartition]->getConnectedTo().push_back(Partition::ConnectionNode(partitionConnected, id, alreadyVisited.back()));
        }
        return make_pair(SearchResult::CONNECTED_TO_PARTITION, alreadyVisited);
    }
}

pair<GPX2::SearchResult, vector<string>> GPX2::DFS_inside(string entry, string exit, CityMap father, Partition* partitionPtr)
{
    //fazer uma busca em profundidade dentro da partição
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
        vector<CityNode::node> edges = father[now]->getEdges();

        for (CityNode::node cn : edges) {

            notAlreadyVisited = (find(alreadyVisited.begin(), alreadyVisited.end(), cn.first) == alreadyVisited.end());
            notToVisit = (find(nextToVisit.begin(), nextToVisit.end(), cn.first) == nextToVisit.end());
            isInPartition = (find(partition.begin(), partition.end(), cn.first) != partition.end());

            if (notAlreadyVisited && notToVisit && isInPartition) {
                nextToVisit.push_back(cn.first);
            }
        }
    }
    return (make_pair((alreadyVisited.back() == exit ? SearchResult::IS_CONNECTED : SearchResult::IS_NOT_CONNECTED), alreadyVisited));
}



void GPX2::eraseSubVector(vector<string>& vec, vector<string>& subvec)
{ // Apaga um subvetor
    for (unsigned i = 0; i < subvec.size(); i++) {
        for (unsigned j = 0; j < vec.size(); j++) {
            if (subvec[i].compare(vec[j]) == 0) {
                vec.erase(vec.begin() + j);
                break;
            }
        }
    }
}

vector<pair<string, string>> GPX2::getEntryAndExitList(Partition* p)
{
    vector<pair<string, string>> entryAndExit;
    vector<string> accessVec = p->getAccessNodes();
    for (unsigned i = 0; i < accessVec.size(); i++) {
        for (unsigned j = 0; j < accessVec.size(); j++) {
            if (i != j) {
                pair<SearchResult, vector<string>> result = DFS_inside(accessVec[i], accessVec[j], red, p);
                pair<SearchResult, vector<string>> resultTmp = DFS_inside(accessVec[i], accessVec[j], blue, p);
                if (result.first == SearchResult::IS_CONNECTED && resultTmp.first == SearchResult::IS_CONNECTED) {

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

int GPX2::partialDistance(string entry, string exit, CityMap father, Partition* partitionPtr)
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
        vector<CityNode::node> edges = father[now]->getEdges();

        for (CityNode::node cn : edges) {

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

void GPX2::printMap(CityMap& graph, std::ostream& stream)
{
    stream << "size: " << graph.size() << endl;
    for (map<string, CityNode*>::iterator it = graph.begin(); it != graph.end(); it++) {
        stream << " " << it->first << " | " << graph[it->first]->getId() << endl;
        if (it->second->getAccess()) {
            stream << "Access: " << true << endl;
        } else {
            stream << "Access: " << false << endl;
        }
        stream << "==================================" << endl;

        stream << "connections: " << it->second->getEdges().size() << endl;
        for (unsigned i = 0; i < it->second->getEdges().size(); i++) {
            stream << "Edge " << i << ": " << graph[it->second->getEdges()[i].first]->getId();
            stream << "\t#\t";
            stream << "Distance: " << it->second->getEdges()[i].second << endl;
        }
        stream << "----------------------------------" << endl;
        stream << endl;
    }

    stream << "______________________________________________________________THE DIVISION______________________________________________________________" << endl;

    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    CityNode* city = graph.begin()->second; // Reduzir chamadas (CityNode dentro do map)

    isAlreadyVisited.push_back(graph.begin()->first);
    stream << graph.begin()->first << endl;
    nextToVisit.push_back(city->getEdges()[0].first);

    while (!nextToVisit.empty()) {

        stream << nextToVisit.front() << endl;
        isAlreadyVisited.push_back(nextToVisit.front());
        city = graph[nextToVisit.front()];
        nextToVisit.pop_front();

        for (CityNode::node n : city->getEdges()) {

            notAlreadyVisited = find(isAlreadyVisited.begin(), isAlreadyVisited.end(), n.first) == isAlreadyVisited.end();
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end();

            if (notAlreadyVisited && notToVisit) {
                nextToVisit.push_back(n.first);
            }
        }
    }
    stream << "size " << isAlreadyVisited.size() << endl;
    stream << "map size " << graph.size() << endl;
}

int GPX2::totalDistance(CityMap& graph) 
{ // Distancia total para percorrer o grafo
    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    int totalDistance{ 0 };
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    CityNode *city = graph.begin()->second, *last = nullptr; // Reduzir chamadas (CityNode dentro do map)
    isAlreadyVisited.push_back(graph.begin()->first);

    nextToVisit.push_back(city->getEdges()[0].first);
    totalDistance += city->getEdges()[0].second;

    while (!nextToVisit.empty()) {
        isAlreadyVisited.push_back(nextToVisit.front());
        city = graph[nextToVisit.front()];
        nextToVisit.pop_front();

        for (CityNode::node n : city->getEdges()) {

            notAlreadyVisited = find(isAlreadyVisited.begin(), isAlreadyVisited.end(), n.first) == isAlreadyVisited.end();
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end();

            if (notAlreadyVisited && notToVisit) {
                nextToVisit.push_back(n.first);
                totalDistance += n.second;
            }
        }
        last = city;
    }
    for (CityNode::node n : last->getEdges()) {
        if (n.first.compare(graph.begin()->first) == 0) {
            totalDistance += n.second;
        }
    }
    return totalDistance;
}

int GPX2::whichPartition(const string id, PartitionMap feasiblePartitions)
{ // Procura em qual partição está a cidade procurada, retorna o ID da partição
    for (auto& p : feasiblePartitions) {
        if (find(p.second->getNodes().begin(), p.second->getNodes().end(), id) != p.second->getNodes().end()) {
            return (p.first);
        }
    }
    return (-1);
}

void GPX2::deleteCityMap(CityMap& m)
{ // deletar o mapa completamente,
    // desalocando os ponteiros tb
    for (auto& it : m) {
        delete it.second;
        it.second = nullptr;
    }
    m.clear();
}

/*  
    -------------------------------------------------------------------------------------
    Fusion utils
*/

void GPX2::checkUnfeasiblePartitions()
{ // Função que irá verificar se foi gerada alguma partição feasible durante a fusão
    for (auto it = unfeasiblePartitions.begin(); it != unfeasiblePartitions.end();) {
        if (checkPartition((*it).second)) { // Checar se é feasible
            // se for irá ser retirada das partições unfeasible e colocada nas partições feasible
            feasiblePartitions.insert(make_pair((*it).first, (*it).second));
            it = unfeasiblePartitions.erase(it);
        } else {
            it++;
        }
    }
}

void GPX2::countConnectedPartitions()
{ // Função que irá realizar a contagem das ligações entre as partições
    for (auto& p : unfeasiblePartitions) {
        map<int, int> countConnected;
        for (auto node : p.second->getConnectedTo()) {
            // Tenta inserir uma nova partição que está conectada
            auto result = countConnected.insert(make_pair(node.connectedPartition, 1));
            // Caso a inserção falhar, a partição já está na lista de conectadas, sendo assim o total de ligações será aumentado
            if (!result.second) {
                countConnected[node.connectedPartition]++;
            }
        }
        // Insere os resultados da contagem
        for (auto node : countConnected) {
            p.second->getConnections().push_back(make_pair(node.first, node.second));
        }
    }
}

void GPX2::fusePartitions()
{ // Realiza a fusão entre as partições, de acordo com as validações já realizadas
    // Início da execução da fusão em si
    for (auto it = fuseWith.begin(); it != fuseWith.end();) {
        // Não precisa mudar o custo pois a checkPartition não verifica isso
        Partition* p1Ptr = unfeasiblePartitions[(*it).partitionId1];
        Partition* p2Ptr = unfeasiblePartitions[(*it).partitionId2];
        vector<string> intermediariesNodes, accessNodesToRemove;

        // Deletar as entradas de conexões (connectTo) diferentes da partição que será fundida
        for (auto itIn = p1Ptr->getConnectedTo().begin(); itIn != p1Ptr->getConnectedTo().end();) {
            if ((*itIn).connectedPartition != (*it).partitionId2) {
                p1Ptr->getConnectedTo().erase(itIn);
            } else {
                itIn++;
            }
        }
        for (auto itIn = p2Ptr->getConnectedTo().begin(); itIn != p2Ptr->getConnectedTo().end();) {
            if ((*itIn).connectedPartition != (*it).partitionId1) {
                p2Ptr->getConnectedTo().erase(itIn);
            } else {
                itIn++;
            }
        }

        for (string nodes : p2Ptr->getNodes()) {
            p1Ptr->getNodes().push_back(nodes);
        }
        for (string access : p2Ptr->getAccessNodes()) {
            p1Ptr->getAccessNodes().push_back(access);
        }

        // Percorrer os nós com ligação na partição que será feita a fusão
        for (auto connectedNode : p2Ptr->getConnectedTo()) {

            // Pegar os nós intermediários (o "caminho" da conexão)
            pair<SearchResult, vector<string>> tmp = DFS_outside(connectedNode.node, unfeasiblePartitions);

            // Apagar os access nodes da lista da partição
            accessNodesToRemove.push_back(connectedNode.node);
            accessNodesToRemove.push_back(connectedNode.connectedNode);

            // Apagar o primeiro pois é o nó access que da origem a busca
            if (!tmp.second.empty()) {
                tmp.second.erase(tmp.second.begin());

                // Apagar o último nó pois é onde ele se conecta na partição que será unida
                if (!tmp.second.empty()) {
                    tmp.second.erase(tmp.second.end());
                }
            }

            // Carrega os nós intermediarios
            intermediariesNodes.insert(intermediariesNodes.end(), tmp.second.begin(), tmp.second.end());
        }

        /*  
            Inserir os nós intermediários na nova partição fundida
            Os nós são a ligação entre a P1 e P2, sendo a conexão de fusão das duas
        */
        p1Ptr->getNodes().insert(p1Ptr->getNodes().end(), intermediariesNodes.begin(), intermediariesNodes.end());

        // Os access nodes que ligavam as duas partições não serão mais access nodes, eles serão removidos
        eraseSubVector(p1Ptr->getAccessNodes(), accessNodesToRemove);

        //atualizar a lista de entradas e saídas
        p1Ptr->setEntryAndExits(getEntryAndExitList(p1Ptr));

        // Apagar tudo
        delete p2Ptr;
        unfeasiblePartitions.erase((*it).partitionId2);

        //apaga a entrada na fuseWith pois a fusão já foi realizada
        it = fuseWith.erase(it);
    }
}

vector<GPX2::UnfeasibleConnection> GPX2::fusionsWithPartition(const int id, vector<GPX2::UnfeasibleConnection>& connections)
{
    vector<GPX2::UnfeasibleConnection> instances;
    for (const auto& con : connections) {
        if (con.partitionId1 == id || con.partitionId2 == id) {
            instances.push_back(con);
        }
    }
    return (instances);
}

void GPX2::generateFusionPairs()
{ // Percorre o mapa com as partições unfeasible para gerar a lista com fusões a serem feitas

    for (auto uF : unfeasiblePartitions) {

        // Pega o retorno da função, IDs das partições e número de conexões entre elas, e salva na estrutura
        UnfeasibleConnection data = whichPartitionToFuseWith(unfeasiblePartitions[uF.first]);

        // Caso seja -1 ele está ligado a uma partição feasible, logo é ignorado
        if (data.partitionId1 != -1) {
            // Carrega o par de partições para fundir
            fuseWith.push_back(UnfeasibleConnection(uF.first, data.partitionId1, data.numberOfConnections));
        }
    }

    /* 
        Irá percorrer a lista fuseWith retirando as entradas iguais

        Ex: 1,2 e 2,1
            É uma situação de igualdade na qual é retirada, sobrando apenas uma
    */
    for (auto itOut = fuseWith.begin(); itOut != fuseWith.end(); itOut++) {
        for (auto itIn = fuseWith.begin(); itIn != fuseWith.end();) {
            if (itOut != itIn) {

                // Método para comparação, caso ocorra a igualdade então é retirado da lista
                if (comparePairInt(*itOut, *itIn)) {
                    itIn = fuseWith.erase(itIn);
                } else {
                    itIn++;
                }
            } else {
                itIn++;
            }
        }
    }

    for (const auto& p : unfeasiblePartitions) {
        // Pega o retorno do método, todas as possíveis fusões daquela partição
        vector<UnfeasibleConnection> tmp = fusionsWithPartition(p.first, fuseWith);

        // Se for menor que 2, logo não é possível que ela seja utilizada em mais de uma fusão.
        if (fuseWith.size() < 2) {
            break;
        }

        // Bloco para validação em caso de "melhor" fusão, compara entre as possíveis fusões e irá manter apenas a com maior número de conexões entre as partições.
        if (!tmp.empty()) {
            int maxCon{ -1 }, maxPos{ -1 };
            for (unsigned i = 0; i < tmp.size(); i++) {
                if (tmp[i].numberOfConnections > maxCon) {
                    maxCon = tmp[i].numberOfConnections;
                    maxPos = i;
                }
            }

            tmp.erase(tmp.begin() + maxPos);

            
            /*vector<UnfeasibleConnection> sameCon;
            int maxCon{ -1 }, maxPos{ -1 };
            for (unsigned i = 0; i < tmp.size(); i++) {
                if (tmp[i].numberOfConnections > maxCon) {
                    sameCon.clear();
                    maxCon = tmp[i].numberOfConnections;
                    maxPos = i;
                    sameCon.push_back(tmp[i]);
                }else if(tmp[i].numberOfConnections == maxCon){
                    sameCon.push_back(tmp[i]);
                }
            }

            if(sameCon.size()!=1){
                
                unsigned numberOfNodes{999999};
                unsigned numberOfNodes{0}; 
                UnfeasibleConnection best{0,0,0};
                for(UnfeasibleConnection uC : sameCon){
                    unsigned p1Nodes{unfeasiblePartitions[uC.partitionId1]->getNodes().size()},p2Nodes{unfeasiblePartitions[uC.partitionId2]->getNodes().size()};
                    unsigned sum{p1Nodes+p2Nodes};
                    //fusão que produzi o maior numero de nodes
                    if(sum>numberOfNodes){
                        numberOfNodes=sum;
                        best = uC;
                    } 

                    //fusão que produzi o maior numero de nodes
                    if(sum<numberOfNodes){
                        numberOfNodes=sum;
                        best = uC;
                    }
                }
                std::cout<<"escolheu a fusion com mais nodes "<<numberOfNodes<<endl; 
                std::cout<<"escolheu a fusion com menos nodes "<<numberOfNodes<<endl;
                tmp.erase(find_if(tmp.begin(),tmp.end(),[&best](UnfeasibleConnection const &uC){return((uC.partitionId1==best.partitionId1) && (uC.partitionId2==best.partitionId2));}));

            }else{
                tmp.erase(tmp.begin() + maxPos);
            } */

            for (auto& unfCon : tmp) {
                for (auto it = fuseWith.begin(); it != fuseWith.end();) {

                    if (unfCon == (*it)) {

                        it = fuseWith.erase(it);
                    } else {
                        it++;
                    }
                }
            }
        }
    }
}

bool GPX2::unfeasiblePartitionsConnected()
{ // Verifica as conexões das partições unfeasible
    vector<string> nodesToCheck;
    bool atLeastOneConnected{ false };

    // Pega todos os nós que serão verificados
    for (auto& it : unfeasiblePartitions) {
        for (string id : it.second->getAccessNodes()) {
            nodesToCheck.push_back(id);
        }
    }

    // Percorre o vetor para verificar as conexões
    for (auto it = nodesToCheck.begin(); it != nodesToCheck.end(); it++) {

        // Percorre fora da partição encontrando onde o nó se liga
        DFS_outside((*it), unfeasiblePartitions, true);
        // Verifica em qual partição o nó está
        int idPartition = whichPartition((*it), unfeasiblePartitions);

        // Verifica qual partição está conectada
        int connectedPartition = unfeasiblePartitions[idPartition]->getConnectedTo().back().connectedPartition;

        // Verifica o ID do nó na qual ela está conectada
        string connectedId = unfeasiblePartitions[idPartition]->getConnectedTo().back().connectedNode;

        // Caso ele esteja conectado a outra partição unfeasible, caso não seja "-1" (conectado com uma partição feasible)
        if (connectedPartition != -1) {
            atLeastOneConnected = true;

            // Insere no vetor de conexões
            unfeasiblePartitions[connectedPartition]->getConnectedTo().push_back(Partition::ConnectionNode(idPartition, connectedId, (*it)));

            // Não precisa verificar os nós que já foram ligados
            nodesToCheck.erase(remove(nodesToCheck.begin(), nodesToCheck.end(), connectedId), nodesToCheck.end());
        } else {
            // Apaga por estar conectado com uma feasible
            unfeasiblePartitions[idPartition]->getConnectedTo().erase(unfeasiblePartitions[idPartition]->getConnectedTo().end());
        }
    }
    return (atLeastOneConnected);
}

GPX2::UnfeasibleConnection GPX2::whichPartitionToFuseWith(Partition* partition)
{ // Retorna um par com a partição que contém mais conexões e o total de conexões

    int partitionId{ -1 }, max{ -1 };
    for (auto p : partition->getConnections()) {
        if (p.second > max) {
            max = p.second;
            partitionId = p.first;
        }
    }
    return (UnfeasibleConnection(partitionId, max, max));
}

bool operator==(const GPX2::UnfeasibleConnection& uf1, const GPX2::UnfeasibleConnection& uf2)
{
    bool first = ((uf1.partitionId1 == uf2.partitionId1) || (uf1.partitionId1 == uf2.partitionId2));
    bool second = ((uf1.partitionId2 == uf2.partitionId1) || (uf1.partitionId2 == uf2.partitionId2));

    return (first && second);
}

GPX2::GPX2() {}

GPX2::~GPX2()
{
    deleteCityMap(red);
    deleteCityMap(blue);
    deleteCityMap(unitedGraph);
    deletePartitionMap(feasiblePartitions);
    deletePartitionMap(unfeasiblePartitions);
    partitionsChoosen.clear();
    feasiblePartitions.clear();
}