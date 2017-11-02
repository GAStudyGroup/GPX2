#include "GPX2.hpp"

Tour GPX2::crossover(Tour redT, Tour blueT, bool legacy)
{
    GPX2 obj;
    // Step 1
    obj.red = obj.tourToMap(redT);
    obj.blue = obj.tourToMap(blueT);

    // Step 2
    if (!legacy) {
        obj.createGhosts();
    }

    // Step 3 
    obj.joinGraphs();

    // Step 4
    obj.cutCommonEdges();

    // Step 5
    obj.findAllPartitions();
    obj.cleanInsideAccess();

    if (obj.allPartitions.size() == 1) {
        obj.deleteAll();
        return ((redT.getFitness() > blueT.getFitness()) ? redT : blueT);
    }

    cout << "all partitions found" << endl;
    for (auto p : obj.allPartitions) {
        for (auto node : p.second->getNodes()) {
            cout << node << " ";
        }
        cout << endl;
        cout << "access nodes: ";
        for (auto anode : p.second->getAccessNodes()) {
            cout << anode << " ";
        }
        cout << endl;
    }

    // Step 6
    obj.checkAllPartitions();
    cout << "unfeasible partitions size before fusion " << obj.unfeasiblePartitions.size() << endl;
    cout << "feasible partitions size before fusion " << obj.allPartitions.size() << endl;

    //fusion code here
    if (!legacy) {
        obj.fusion();
    }

    cout << "unfeasible partitions size after fusion " << obj.unfeasiblePartitions.size() << endl;
    cout << "feasible partitions size after fusion " << obj.allPartitions.size() << endl;

    // Step 7
    obj.choose();

    // Step 8
    obj.buildOffspring();

    Tour t;

    if (obj.offspringChoosen == Parent::RED) {
        if (!legacy) {
            obj.removeGhosts(obj.red);
        }
        // Step 9
        t = obj.mapToTour(obj.red);
    } else {
        if (!legacy) {
            obj.removeGhosts(obj.blue);
        }
        // Step 9
        t = obj.mapToTour(obj.blue);
    }

    // Deletar as coisas
    obj.deleteAll();
    return t;
}

// STEP 1 - MAPEAR O TOUR

GPX2::CityMap GPX2::tourToMap(Tour& t)
{ // Mapear o tour para um grafo com ligações entre os nós

    if (t.getRoute().empty()) {
        exit(0);
    }

    map<string, CityNode*> aux; // Mapa com as conexões dos nodes
    double dist = 0;
    vector<City> cities{ t.getRoute() };

    CityNode* prev = new CityNode(to_string(cities.at(0).getId()), cities.at(0).getX(), cities.at(0).getY()); // ponto anterior ao atual dentro do for
    CityNode* first = prev; // referência do primeiro acesso

    aux.insert(make_pair(first->getId(), first)); // gera o mapa e insere o primeiro dentro dele

    for (unsigned i = 1; i < t.getRoute().size(); i++) { // percorre o vetor a partir do segundo elemento, o primeiro já foi transformado
        CityNode* cn = new CityNode(to_string(cities.at(i).getId()), cities.at(i).getX(), cities.at(i).getY()); // gera um node com o segundo elemento

        aux.insert(make_pair(cn->getId(), cn)); // insere o node dentro do mapa
        dist = distance(prev->getX(), prev->getY(), cn->getX(), cn->getY());
        cn->addEdge(CityNode::node(prev->getId(), dist)); // adiciona ao node atual as arestas de conexão

        dist = distance(cn->getX(), cn->getY(), prev->getX(), prev->getY());
        prev->addEdge(CityNode::node(cn->getId(), dist)); // adiciona ao node anterior o atual como um próx (lista duplamente encadeada)

        prev = cn; // o anterior recebe o atual para continuar o for
    }

    dist = distance(prev->getX(), prev->getY(), first->getX(), first->getY());
    first->addEdge(CityNode::node(prev->getId(), dist)); // o primeiro recebe o atual ao sair do for, completando os ligamentos das arestas

    dist = distance(first->getX(), first->getY(), prev->getX(), prev->getY());
    prev->addEdge(CityNode::node(first->getId(), dist)); // o atual recebe o primeiro para completar os ligamentos

    return (aux); // retorna o mpaa com os nodes já instanciados e adicionados
}

// -----------------------------------------------------------------------------

// STEP 2 - Criar e inserir ghosts
void GPX2::createGhosts()
{
    set<string> isGhost;
    // The SET containner does not allow repeated values inside yourself

    for (auto& city : red) {

        string idKey = city.first;
        if (idKey.find(ghostToken) == string::npos) { // the node is not a ghost

            for (unsigned i = 0; i < 2; i++) {
                isGhost.insert(city.second->getEdges()[i].first);
                isGhost.insert(blue[idKey]->getEdges()[i].first);
            }

            if (isGhost.size() == 4) { // node with degree 4
                string ghostID = idKey + ghostToken;
                double x = city.second->getX(), y = city.second->getY();

                CityNode* ghostNode = new CityNode(ghostID, x, y);
                insertGhost(idKey, red, ghostNode);

                ghostNode = new CityNode(ghostID, x, y);
                insertGhost(idKey, blue, ghostNode);
            }

            isGhost.clear();
        }
    }
}

void GPX2::insertGhost(string& id, CityMap& tour, CityNode* ghost)
{
    CityNode::node edgeFirst = tour[id]->getEdges()[0]; // edge que será mandado para o ghost
    tour[id]->deleteEdge(0); // delete edge
    tour[id]->addEdge(CityNode::node(ghost->getId(), 0)); // adiciona ao node "REAL" o edge de ligação com o ghost

    for (unsigned i = 0; i < 2; i++) {
        // percorre os edges de ligação com o nó que foi para o ghost
        CityNode::node edge = tour[edgeFirst.first]->getEdges()[i];

        if (edge.first.compare(id) == 0) { // verifica se é a ligação

            // adiciona a ligação ao ghost e deleta do "REAL"
            tour[edgeFirst.first]->deleteEdge(i);
            tour[edgeFirst.first]->addEdge(CityNode::node(ghost->getId(), edgeFirst.second));
        }
    }

    ghost->addEdge(edgeFirst); // faz o ghost se ligar ao edge
    ghost->setAccess(true);
    ghost->addEdge(CityNode::node(tour[id]->getId(), 0)); // adiciona o REAL como um edge do ghost

    tour.insert(make_pair(ghost->getId(), ghost)); // insere o ghost no mapPai
}

// -----------------------------------------------------------------------------

// STEP 3 - Criar GU

void GPX2::joinGraphs()
{ // executa a união dos dois grafos, gerando Gu

    vector<string> cityList, cityList2;
    for (auto node : red) {
        cityList.push_back(node.first);
    }
    for (auto node : blue) {
        cityList2.push_back(node.first);
    }

    for (string id : cityList) {
        CityNode* c = red[id];
        // criar a entrada no map da união
        unitedGraph.insert(
            make_pair((c->getId()), new CityNode((c->getId()), c->getX(), c->getY())));
        // colocar as edges no map da união
        for (CityNode::node n : red[(c->getId())]->getEdges()) {
            unitedGraph[(c->getId())]->addEdge(make_pair(n.first, n.second));
        }
        for (CityNode::node n : blue[(c->getId())]->getEdges()) {
            unitedGraph[(c->getId())]->addEdge(make_pair(n.first, n.second));
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 4 - Criar GU' (cortar arestas iguais)

void GPX2::cutCommonEdges()
{ // executa o processo de "cortar" as arestas iguais
    // entre os pais, a partir do grafo da união,
    // gerando o Gu'
    for (CityMap::iterator it = unitedGraph.begin();
         it != unitedGraph.end(); it++) { // percorre todas as entradas do Gu

        vector<CityNode::node>& vec = it->second->getEdges(); // Carrega o vetor com as arestas contidas
        // naquela entrada do mapa

        for (unsigned i = 0; i < vec.size(); i++) { // percorre o vetor de arestas

            int commonEdges{ 0 }; // contagem de vértices iguais, caso contenha dois
            // iguais será executado um "corte"

            int last{ -1 }; // posição da primeira instância do no repetido

            for (unsigned j = 0; j < vec.size(); j++) { // percorre o vetor de arestas

                if (vec.at(i).first == vec.at(j).first) { // verifica igualdade
                    commonEdges++;
                    last = j;

                    if (commonEdges > 1) { // caso exista mais do que uma instância do
                        // vértice então executa o corte

                        vec.at(i).second = 0; // Distância entre os vértices = 0 para simbolizar o corte
                        it->second->deleteEdge(
                            last); // Deleta a primeira instância do vértice
                        it->second->setAccess(
                            true); // Altera o acesso do vértice (entrada e saída)
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 5 - Partições

vector<string> GPX2::findPartition(const string nodeOne)
{ // a partir do valor passado
    // irá executar uma busca em
    // Gu' para encontrar as
    // partições
    // nodeOne funciona como uma raiz, que será o ponto inicial da busca, achando
    // a partição onde ele está contido
    // retorna um vetor com os IDs das cidades que compoem a partição

    CityNode* root; // nó para verificação
    vector<string> idAlreadyVisited; // lista dos nós já visitados
    deque<string> nextToVisit; // lista dos próximos a visitar

    vector<string> partition;

    bool notAlreadyVisited{ false };
    bool notToVisit{ false };
    bool notCut{ false };

    nextToVisit.push_back(nodeOne); // primeiro a ser visitado

    while (!nextToVisit.empty()) { // enquanto a não for vazio

        root = unitedGraph[nextToVisit.front()]; // raiz para execução

        nextToVisit.pop_front(); // após "pegar" para execução, limpar o nó da lista

        partition.push_back(root->getId()); // coloca o nó na partição

        idAlreadyVisited.push_back(root->getId()); // Nó na lista de "já visitados"

        for (CityNode::node n :
            root->getEdges()) { // irá percorrer as arestas ligadas

            notAlreadyVisited = find(idAlreadyVisited.begin(), idAlreadyVisited.end(), n.first) == idAlreadyVisited.end(); // verificar se a aresta não foi visitada

            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end(); // verificar se não está na lista para ser visitada

            notCut = n.second != 0; // verificar se ela não foi cortada

            if (notAlreadyVisited && notToVisit && notCut) { // caso todas as condições estejam TRUE
                nextToVisit.push_back(
                    n.first); // coloca na lista dos proximos a serem visitados
            }
        }
    }
    return (partition); // retornar vetor com IDs da partição
}

void GPX2::findAllPartitions()
{ // irá percorrer todo o grafo
    // encotrando as partições
    // presentes nele
    int id{ 1 };
    vector<vector<string>> partitions;
    vector<string> cities;

    //pegar a lista de todas as cidades
    for (auto node : unitedGraph) {
        cities.push_back(node.first);
    }

    while (!cities.empty()) { // enquanto a lista de cidades ainda existir

        partitions.push_back(findPartition(cities.front())); // insere o retorno da função na lista de partições

        for (string i : partitions.back()) {
            cities.erase(remove(cities.begin(), cities.end(), i), cities.end());
        }
    }

    for (vector<string> vi : partitions) { // retirar partições com tamanho 1
        if (vi.size() != 1) {
            vector<string> accessNodes;
            for (string id : vi) {
                if (unitedGraph[id]->getAccess()) {
                    accessNodes.push_back(id);
                }
            }
            allPartitions.insert(make_pair(id, new Partition(id, vi, accessNodes)));
            id++;
        }
    }
}

void GPX2::cleanInsideAccess()
{
    for (auto& p : allPartitions) {
        vector<string> tmp;
        for (string id : p.second->getAccessNodes()) {
            pair<SearchResult, vector<string>> result = DFS_outside(id, allPartitions);
            if (result.first == SearchResult::CONNECTED_TO_PARTITION) {
                tmp.push_back(id);
            } else {
                if (!result.second.empty()) {
                    result.second.erase(result.second.begin());
                }
                if (!result.second.empty()) {
                    result.second.erase(result.second.end());
                }
                p.second->getNodes().insert(p.second->getNodes().end(), result.second.begin(), result.second.end());
            }
        }
        p.second->setAccessNodes(tmp);
    }
}

// -----------------------------------------------------------------------------

// STEP 6 - Checar recombinantes

void GPX2::checkAllPartitions()
{
    //verifica todas as partições
    for (auto p : allPartitions) {
        //se não for uma partição recombinante ele deleta da lista de partições
        if (!checkPartition(p.second)) {
            unfeasiblePartitions.insert(make_pair(p.first, p.second));
            allPartitions.erase(p.first);
        }
    }
}

bool GPX2::checkPartition(Partition* partition)
{
    unsigned size = partition->getAccessNodes().size();
    vector<string> redNodes, blueNodes;
    redNodes = blueNodes = partition->getNodes();
    if (!(size % 2 == 0)) {
        //não é uma partição recombinante pois não possui uma entrada para cada saida.
        return (false);
    } else {
        vector<string> nodesInPartition = partition->getNodes();
        pair<string, string> access;
        //for (unsigned i = 0; i < size / 2; i++) {
        for (unsigned i = 0; i < size; i++) {
            //encontrando entrada e saida no red e verifica se elas existem
            /* access.first = partition->getAccessNodes()[2 * i]; */
            access.first = partition->getAccessNodes()[i];
            bool foundConnected{ false };
            for (unsigned j = 0; j < size; j++) {

                if (i != j) {
                    vector<string> nodesVisited;
                    if (DFS_inside(access.first, partition->getAccessNodes()[j], red, partition, nodesVisited) == SearchResult::IS_CONNECTED) {
                        access.second = partition->getAccessNodes()[j];
                        foundConnected = true;
                        eraseSubVector(redNodes, nodesVisited);
                        break;
                    }
                }
            }
            if (!foundConnected) {
                return (false);
            }
            //verifica no blue se os mesmos pontos de entrada e saida funcionam
            {
                vector<string> nodesVisited;
                if (DFS_inside(access.first, access.second, blue, partition, nodesVisited) == SearchResult::IS_CONNECTED) {
                    eraseSubVector(blueNodes, nodesVisited);
                } else {
                    return (false);
                }
            }
        }
        //depois de encontrar todas as entradas e saidas, é necessário verificar se todos os nós da partição foram percorridos pelos dois pais
        if (redNodes.empty() && blueNodes.empty()) {
            return (true);
        } else {
            return (false);
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 7 - Escolher partições para o filho

void GPX2::choose()
{
    for (auto p : allPartitions) {
        double totalRed{ 0.0 }, totalBlue{ 0.0 };
        vector<string> accessVec = p.second->getAccessNodes();
        for (unsigned i = 0; i < accessVec.size() / 2; i++) {
            totalRed = parcialDistance(accessVec.at(2 * i), accessVec.at((2 * i) + 1), red, p.second);
            totalBlue = parcialDistance(accessVec.at(2 * i), accessVec.at((2 * i) + 1), blue, p.second);
        }
        if (totalRed < totalBlue) {
            partitionsChoosen.push_back(Parent::RED);
        } else {
            partitionsChoosen.push_back(Parent::BLUE);
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 8 - Gerar o mapa do filho

void GPX2::buildOffspring()
{
    CityMap offspring = red; // red é o pivo.
    int index{ 0 };

    for (auto& allP : allPartitions) {
        if (partitionsChoosen[index] == Parent::BLUE) { // se o Blue for melhor que o Red naquela partição
            for (string s : allP.second->getNodes()) {
                delete red.at(s);
                red.erase(s);

                CityNode* newNode = new CityNode(blue.at(s)->getId(), blue.at(s)->getX(), blue.at(s)->getY());
                newNode->setEdges(blue.at(s)->getEdges());
                red.insert(make_pair(s, newNode));
            }
        } else {
            for (string s : allP.second->getNodes()) {
                delete blue.at(s);
                blue.erase(s);

                CityNode* newNode = new CityNode(red.at(s)->getId(), red.at(s)->getX(), red.at(s)->getY());
                newNode->setEdges(red.at(s)->getEdges());
                blue.insert(make_pair(s, newNode));
            }
        }
        index++;
    }
    offspringChoosen = ((totalDistance(red) < totalDistance(blue)) ? (Parent::RED) : (Parent::BLUE));
    /* offspringChoosen = Parent::RED; */
}

void GPX2::removeGhosts(CityMap& graph)
{
    for (auto node : graph) {
        size_t index = node.first.find(ghostToken);
        if (index != string::npos) {
            //pegar o id do nó sem o token de ghost
            string id = node.first;
            id.erase(index, ghostToken.size());

            //pegar os dois nós que estão ligados ao nó normal e ao ghost

            //iterando pelos edges do nó normal
            vector<CityNode::node> edges = graph[id]->getEdges();
            unsigned edgeToDelete{ 0 };
            CityNode::node prev, next;
            for (unsigned i = 0; i < edges.size(); i++) {
                //se encontrar a edge ligada ao nó ghost
                if (edges.at(i).first.compare(node.first) == 0) {
                    edgeToDelete = i;
                } else {
                    prev = edges.at(i);
                }
            }
            graph[id]->deleteEdge(edgeToDelete);

            edges = graph[node.first]->getEdges();

            for (unsigned i = 0; i < edges.size(); i++) {
                //encontrar o nó que não é o normal na lista de edges
                if (!(edges.at(i).first.compare(id) == 0)) {
                    next = edges.at(i);
                    break;
                }
            }

            //ligar o nó normal no next
            graph[id]->addEdge(next);

            //arrumar as edges do next
            edges = graph[next.first]->getEdges();
            for (unsigned i = 0; i < edges.size(); i++) {
                //encontrou a edge que referencia ao ghost
                if (edges.at(i).first.compare(node.first) == 0) {
                    edgeToDelete = i;
                    break;
                }
            }
            graph[next.first]->deleteEdge(edgeToDelete);

            graph[next.first]->addEdge(make_pair(id, next.second));

            //deletar o nó ghost
            delete graph[node.first];
            graph.erase(node.first);
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 9 - Linearizar o mapa do filho

Tour GPX2::mapToTour(CityMap& mapOffspring)
{
    Tour offspring;
    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;

    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    CityNode* city = mapOffspring.begin()->second; // Reduzir chamadas (CityNode dentro do map)

    isAlreadyVisited.push_back(mapOffspring.begin()->first);

    offspring.getRoute().push_back(City(stoi(city->getId()), city->getX(), city->getY())); // já foi visitado então entra no filho

    nextToVisit.push_back(city->getEdges()[0].first);

    while (!nextToVisit.empty()) {

        isAlreadyVisited.push_back(nextToVisit.front());
        city = mapOffspring[nextToVisit.front()];
        nextToVisit.pop_front();

        offspring.getRoute().push_back(City(stoi(city->getId()), city->getX(), city->getY()));

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

vector<string> GPX2::cityToString(vector<City> cityList)
{
    vector<string> tmp;
    for (City c : cityList) {
        tmp.push_back(to_string(c.getId()));
    }
    return tmp;
}

void GPX2::deleteAll()
{
    deleteCityMap(red);
    deleteCityMap(blue);
    deleteCityMap(unitedGraph);
    deletePartitionMap(allPartitions);
    deletePartitionMap(unfeasiblePartitions);
    partitionsChoosen.clear();
    allPartitions.clear();
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
            partitions[idPartition]->getConnectedTo().push_back(make_pair(partitionConnected, make_pair(id, alreadyVisited.back())));
        }
        return make_pair(SearchResult::CONNECTED_TO_PARTITION, alreadyVisited);
    }
}

GPX2::SearchResult GPX2::DFS_inside(string entry, string exit, CityMap father, Partition* partitionPtr, vector<string>& returnVector)
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

    returnVector = alreadyVisited;
    return (alreadyVisited.back() == exit ? SearchResult::IS_CONNECTED : SearchResult::IS_NOT_CONNECTED);
}

double GPX2::distance(double x1, double y1, double x2, double y2)
{
    return (sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2)));
}

void GPX2::eraseSubVector(vector<string>& vec, vector<string>& subvec)
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

double GPX2::parcialDistance(string entry, string exit, CityMap father, Partition* partitionPtr)
{
    //fazer uma busca em profundidade dentro da partição
    string now;
    vector<string> alreadyVisited;
    deque<string> nextToVisit;
    vector<string> partition = partitionPtr->getNodes();
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };
    bool isInPartition{ false };

    double totalDistance{ 0 };

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

    return totalDistance;
}

void GPX2::printMap(CityMap& graph)
{
    /* for (map<string, CityNode*>::iterator it = m.begin(); it != m.end(); it++) {
        cout << " " << it->first << " | " << m[it->first]->getId() << endl;
        cout << "Acess: " << it->second->getAccess() << endl;
        cout << "==================================" << endl;

        for (int i = 0; i < it->second->getEdges().size(); i++) {
            cout << "Edge " << i << ": " << m[it->second->getEdges().at(i).first]->getId();
            cout << "\t#\t";
            cout << "Distance: " << it->second->getEdges().at(i).second << endl;
        }
        cout << "----------------------------------" << endl;
        cout << endl;
    } */

    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    CityNode* city = graph.begin()->second; // Reduzir chamadas (CityNode dentro do map)

    isAlreadyVisited.push_back(graph.begin()->first);
    cout << graph.begin()->first << " ";
    nextToVisit.push_back(city->getEdges()[0].first);
    cout << city->getEdges()[0].first << " ";

    while (!nextToVisit.empty()) {

        isAlreadyVisited.push_back(nextToVisit.front());
        city = graph[nextToVisit.front()];
        nextToVisit.pop_front();

        for (CityNode::node n : city->getEdges()) {

            notAlreadyVisited = find(isAlreadyVisited.begin(), isAlreadyVisited.end(), n.first) == isAlreadyVisited.end();
            notToVisit = find(nextToVisit.begin(), nextToVisit.end(), n.first) == nextToVisit.end();

            if (notAlreadyVisited && notToVisit) {
                nextToVisit.push_back(n.first);
                cout << n.first << " ";
            }
        }
    }
    cout << endl;
}

double GPX2::totalDistance(CityMap& graph)
{
    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    double totalDistance{ 0.0 };
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
        if (n.first.compare(isAlreadyVisited.front()) == 0) {
            totalDistance += n.second;
        }
    }
    return totalDistance;
}

int GPX2::whichPartition(const string id, PartitionMap allPartitions)
{ // Procura em qual partição está a cidade procurada, retorna o ID da partição
    for (auto& p : allPartitions) {
        if (find(p.second->getNodes().begin(), p.second->getNodes().end(), id) != p.second->getNodes().end()) {
            return (p.first);
        }
    }
    return (-1);
}

bool GPX2::unfeasiblePartitionsConnected()
{
    vector<string> nodesToCheck;
    bool atLeastOneConnected;
    //pegar todos os nós a serem verificados
    for (auto& it : unfeasiblePartitions) {
        for (string id : it.second->getAccessNodes()) {
            nodesToCheck.push_back(id);
        }
    }

    for (auto it = nodesToCheck.begin(); it != nodesToCheck.end(); it++) {
        DFS_outside((*it), unfeasiblePartitions, true);
        int idPartition = whichPartition((*it), unfeasiblePartitions);

        int connectedPartition = unfeasiblePartitions.at(idPartition)->getConnectedTo().back().first;

        string connectedId = unfeasiblePartitions.at(idPartition)->getConnectedTo().back().second.second;
        //se ele estiver connectado a outra partição unfeasible
        if (connectedPartition != -1) {
            atLeastOneConnected = true;
            //ligar uma partição a outra
            unfeasiblePartitions.at(connectedPartition)->getConnectedTo().push_back(make_pair(idPartition, make_pair(connectedId, (*it))));

            //não precisa verificar os nós que já foram ligados
            nodesToCheck.erase(remove(nodesToCheck.begin(), nodesToCheck.end(), connectedId), nodesToCheck.end());
        } else {
            unfeasiblePartitions.at(idPartition)->getConnectedTo().erase(unfeasiblePartitions.at(idPartition)->getConnectedTo().end());
        }
    }
    return (atLeastOneConnected);
}

void GPX2::countConnectedPartitions()
{
    for (auto& p : unfeasiblePartitions) {
        map<int, int> countConnected;
        for (auto node : p.second->getConnectedTo()) {
            //tenta inserir uma nova partição conectada
            auto result = countConnected.insert(make_pair(node.first, 1));
            //se falhar a inserção quer dizer que a partição já está lá, então incrementa o número de vezes que as ligações ocorrem
            if (!result.second) {
                countConnected.at(node.first)++;
            }
        }
        for (auto node : countConnected) {
            p.second->getConnections().push_back(make_pair(node.first, node.second));
        }
    }
}

pair<int, int> GPX2::whichPartitionToFuseWith(Partition* partition)
{
    int partitionId{ -1 }, max{ -1 };
    for (auto p : partition->getConnections()) {
        if (p.second > max) {
            max = p.second;
            partitionId = p.first;
        }
    }
    return (make_pair(partitionId, max));
}

void GPX2::fusePartitions()
{
    vector<int> partitionsToCheck;
    vector<pair<int, int>> fuseWith;
    vector<int> numberOfConnections;
    for (auto p : unfeasiblePartitions) {
        partitionsToCheck.push_back(p.first);
    }
    for (auto it = partitionsToCheck.begin(); it != partitionsToCheck.end(); it++) {
        //ela pode se fundir com a partição se retornar algo diferente de -1
        pair<int, int> data = whichPartitionToFuseWith(unfeasiblePartitions.at((*it)));
        if (data.first != -1) {
            fuseWith.push_back(make_pair((*it), data.first));
            numberOfConnections.push_back(data.second);
            partitionsToCheck.erase(remove(partitionsToCheck.begin(), partitionsToCheck.end(), data.first), partitionsToCheck.end());
        }
    }

    //partição que for utilizada na fusão não pode ser fundida com outra
    for (unsigned i = 0; i < fuseWith.size(); i++) {
        for (unsigned j = 0; j < fuseWith.size(); j++) {
            if (fuseWith.at(i).first == fuseWith.at(j).second) {
                cout << "erasing fusion of " << fuseWith.at(i).first << " with " << fuseWith.at(j).second << endl;
                fuseWith.erase(fuseWith.begin() + i);
            }
        }
    }

    //verificar se a mesma partição será utilizada em duas fusões
    for (unsigned i = 0; i < fuseWith.size(); i++) {
        for (unsigned j = 0; j < fuseWith.size(); j++) {
            //não verificar o nó com ele mesmo
            if (i != j) {
                //partição será utilizada em duas fusões
                if (fuseWith.at(i).second == fuseWith.at(j).second) {
                    cout << "particao " << fuseWith.at(i).second << " sera utiliza em duas ou mais fusões!" << endl;
                    cout << "\tparticao " << fuseWith.at(i).first << " conectou " << numberOfConnections.at(i) << " vezes" << endl;
                    cout << "\tparticao " << fuseWith.at(j).first << " conectou " << numberOfConnections.at(j) << " vezes" << endl;
                    if (numberOfConnections.at(i) < numberOfConnections.at(j)) {
                        numberOfConnections.erase(numberOfConnections.begin() + i);
                        fuseWith.erase(fuseWith.begin() + i);
                    } else {
                        numberOfConnections.erase(numberOfConnections.begin() + j);
                        fuseWith.erase(fuseWith.begin() + j);
                    }
                }
            }
        }
    }

    //fazer a fusão das partições
    //unir os nodes
    //unir os accessNodes
    //colocar os nós entre os access na lista de nós da partição
    //tirar os nós da connections da accessNodes
    //resetar o connectedTo
    //resetar connections
    //deletar a segunda partição
    for (auto p : fuseWith) {
        cout << "fusing partition " << p.first << " with " << p.second << endl;
        //não precisa mudar o custo pois a checkPartition não verifica isso
        Partition* p1Ptr = unfeasiblePartitions.at(p.first);
        Partition* p2Ptr = unfeasiblePartitions.at(p.second);
        vector<string> intermediariesNodes, accessNodesToRemove;
        //deletar as connectedTo diferentes da partição que será fundida
        for (auto it = p1Ptr->getConnectedTo().begin(); it != p1Ptr->getConnectedTo().end();) {
            if ((*it).first != p.second) {
                p1Ptr->getConnectedTo().erase(it);
            } else {
                it++;
            }
        }
        for (auto it = p2Ptr->getConnectedTo().begin(); it != p2Ptr->getConnectedTo().end();) {
            if ((*it).first != p.first) {
                p2Ptr->getConnectedTo().erase(it);
            } else {
                it++;
            }
        }

        for (string nodes : p2Ptr->getNodes()) {
            p1Ptr->getNodes().push_back(nodes);
        }
        for (string access : p2Ptr->getAccessNodes()) {
            p1Ptr->getAccessNodes().push_back(access);
        }
        for (auto connectedNode : p2Ptr->getConnectedTo()) {
            /* 
                PEGAR APENAS OS NÓS QUE LIGAM NA PARTIÇÃO QUE VAI SER FEITA A FUSÃO
            */
            //pegar os nós intermediários
            pair<SearchResult, vector<string>> tmp = DFS_outside(connectedNode.second.first, unfeasiblePartitions);

            //apagar os access nodes da lista da partição
            accessNodesToRemove.push_back(connectedNode.second.first);
            accessNodesToRemove.push_back(connectedNode.second.second);
            //apagar o primeiro pois é o nó access que da origem a busca
            if (!tmp.second.empty()) {
                tmp.second.erase(tmp.second.begin());
            }
            //apagar o último nó pois é onde ele se conecta na partição que será unida
            if (!tmp.second.empty()) {
                tmp.second.erase(tmp.second.end());
            }

            intermediariesNodes.insert(intermediariesNodes.end(), tmp.second.begin(), tmp.second.end());
        }

        //colocar os nós intermediários nos nós da partição
        p1Ptr->getNodes().insert(p1Ptr->getNodes().end(), intermediariesNodes.begin(), intermediariesNodes.end());

        eraseSubVector(p1Ptr->getAccessNodes(), accessNodesToRemove);

        p1Ptr->getConnectedTo().clear();
        p1Ptr->getConnections().clear();

        delete p2Ptr;
        unfeasiblePartitions.erase(p.second);
    }
}

vector<string> GPX2::DFS_outside_get_nodes(string id, PartitionMap partitions)
{
    string now;
    int idPartition = whichPartition(id, partitions);
    vector<string> partition = partitions[idPartition]->getNodes();
    vector<string> alreadyVisited;
    deque<string> nextToVisit;
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

    return (alreadyVisited);
}

void GPX2::fusion()
{
    bool atLeastOneConnected{ false };

    //enquanto houver mais que uma partição para fazer a fusão e tiver uma partição conectada com outra
    while ((unfeasiblePartitions.size() > 1)) {
        cout << "unfeasible size begin " << unfeasiblePartitions.size() << endl;
        //verifica quais partições estão conectadas
        atLeastOneConnected = unfeasiblePartitionsConnected();
        if (!atLeastOneConnected) {
            cout << "saiu no break" << endl;
            break;
        }
        //contas quantas vezes cada partição se conecta com outra unfeasible
        countConnectedPartitions();

        //faz a fusão das partições
        fusePartitions();
        //se a fusão der certo e a partição se tornar feasible ela passa para a allPartitions novamente
        checkUnfeasiblePartitions();
        cout << "unfeasible size end " << unfeasiblePartitions.size() << endl;
        cout << "feasible partitions size " << allPartitions.size() << endl;
    }
}

void GPX2::checkUnfeasiblePartitions()
{
    for (auto p : unfeasiblePartitions) {
        cout << "checking feasible " << p.first << " result " << checkPartition(p.second) << endl;
        if (checkPartition(p.second)) {
            cout << "fusion gerou uma particao recombinante" << endl;
            allPartitions.insert(make_pair(p.first, p.second));
            unfeasiblePartitions.erase(p.first);
        }
    }
}