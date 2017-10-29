#include "GPX2.hpp"

Tour GPX2::crossover(Tour red, Tour blue)
{
    partitionMap allPartitions;
    cityMap unitedGraph;

    // Step 1
    cityMap redMap = tourToMap(red);
    cityMap blueMap = tourToMap(blue);

    // Step 2
    createGhosts(redMap, blueMap);

    // Step 3
    joinGraphs(redMap, blueMap, unitedGraph);

    // Step 4
    cutCommonEdges(unitedGraph);

    // Step 5
    findAllPartitions(unitedGraph, allPartitions);
    cleanInsideAccess(unitedGraph, allPartitions);

    // Step 6
    checkAllPartitions(redMap, blueMap, unitedGraph, allPartitions);

    // Step 7
    vector<int> partitionsChoosen = choose(redMap, blueMap, allPartitions);

    // Step 8
    buildOffspring(partitionsChoosen, allPartitions, redMap, blueMap);
    removeGhosts(redMap);

    // Step 9
    Tour t = mapToTour(redMap);

    // Deletar as coisas
    deleteMap(redMap);
    deleteMap(blueMap);
    deleteMap(unitedGraph);

    partitionsChoosen.clear();
    allPartitions.clear();

    return t;
}

// STEP 1 - MAPEAR O TOUR

GPX2::cityMap GPX2::tourToMap(Tour& t)
{ // Mapear o tour para um grafo com ligações entre os nós

    if (t.getTour().empty()) {
        exit(0);
    }

    map<string, CityNode*> aux; // Mapa com as conexões dos nodes
    double dist = 0;
    vector<City> cities{ t.getTour() };

    CityNode* prev = new CityNode(to_string(cities.at(0).getId()), cities.at(0).getX(), cities.at(0).getY()); // ponto anterior ao atual dentro do for
    CityNode* first = prev; // referência do primeiro acesso

    aux.insert(make_pair(first->getId(), first)); // gera o mapa e insere o primeiro dentro dele

    for (int i = 1; i < t.getTour().size(); i++) { // percorre o vetor a partir do segundo elemento, o primeiro já foi transformado
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

void GPX2::createGhosts(cityMap& red, cityMap& blue)
{
    set<string> isGhost;
    // The SET containner does not allow repeated values inside yourself

    for (auto& city : red) {

        string idKey = city.first;
        if (idKey.find("-") == string::npos) { // the node is not a ghost

            for (unsigned i = 0; i < 2; i++) {
                isGhost.insert(city.second->getEdges()[i].first);
                isGhost.insert(blue[idKey]->getEdges()[i].first);
            }

            if (isGhost.size() == 4) { // node with degree 4
                string ghostID = idKey + "-";
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

void GPX2::insertGhost(string& id, cityMap& tour, CityNode* ghost)
{
    CityNode::node edgeFirst = tour[id]->getEdges()[0]; // edge que será mandado para o ghost
    tour[id]->deleteEdge(0); // delete edge
    tour[id]->addEdge(CityNode::node(ghost->getId(), 0)); // adiciona ao node "REAL" o edge de ligação com o ghost

    for (unsigned i = 0; i < 2; i++) {
        // percorre os edges de ligação com o nó que foi para o ghost
        CityNode::node edge = tour[edgeFirst.first]->getEdges()[i];

        if (edge.first.compare(id) == 0) { // verifica se é a ligação

            // adiciona a ligação ao ghost e deleta do "REAL"
            double distTmp = edge.second;

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

void GPX2::joinGraphs(
    cityMap& red, cityMap& blue, cityMap& unitedGraph)
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

void GPX2::cutCommonEdges(cityMap& unitedGraph)
{ // executa o processo de "cortar" as arestas iguais
    // entre os pais, a partir do grafo da união,
    // gerando o Gu'
    for (cityMap::iterator it = unitedGraph.begin();
         it != unitedGraph.end(); it++) { // percorre todas as entradas do Gu

        vector<CityNode::node>& vec = it->second->getEdges(); // Carrega o vetor com as arestas contidas
        // naquela entrada do mapa

        for (int i = 0; i < vec.size(); i++) { // percorre o vetor de arestas

            int commonEdges{ 0 }; // contagem de vértices iguais, caso contenha dois
            // iguais será executado um "corte"

            int last{ -1 }; // posição da primeira instância do no repetido

            for (int j = 0; j < vec.size(); j++) { // percorre o vetor de arestas

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

vector<string> GPX2::findPartition(const string nodeOne, cityMap unitedGraph)
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

void GPX2::findAllPartitions(cityMap unitedGraph, partitionMap& allPartitions)
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

        partitions.push_back(findPartition(cities.front(), unitedGraph)); // insere o retorno da função na lista de partições

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
            allPartitions.insert(make_pair(id, Partition(id, vi, accessNodes)));
            id++;
        }
    }
}

void GPX2::cleanInsideAccess(cityMap unitedGraph, partitionMap& allPartitions)
{
    for (auto& p : allPartitions) {
        vector<string> tmp;
        for (string id : p.second.getAccessNodes()) {
            if (DFS_outside(id, unitedGraph, allPartitions) == CONNECTED_TO_PARTITION) {
                tmp.push_back(id);
            }
        }
        p.second.setAccessNodes(tmp);
    }
}

// -----------------------------------------------------------------------------

// STEP 6 - Checar recombinantes

void GPX2::checkAllPartitions(cityMap red, cityMap blue, cityMap& unitedGraph, partitionMap& allPartitions)
{
    //verifica todas as partições
    for (auto p : allPartitions) {
        //se não for uma partição recombinante ele deleta da lista de partições
        if (!checkPartition(red, blue, unitedGraph, p.second)) {
            allPartitions.erase(p.first);
        }
    }
}

bool GPX2::checkPartition(cityMap red, cityMap blue, cityMap& unitedGraph, Partition& partition)
{
    unsigned size = partition.getAccessNodes().size();
    vector<string> redNodes, blueNodes;
    redNodes = blueNodes = partition.getNodes();
    if (!(size % 2 == 0)) {
        //não é uma partição recombinante pois não possui uma entrada para cada saida.
        return (false);
    } else {
        vector<string> nodesInPartition = partition.getNodes();
        bool foundExit{ false };
        pair<string, string> access;
        for (unsigned i = 0; i < size / 2; i++) {
            //encontrando entrada e saida no red e verifica se elas existem
            access.first = partition.getAccessNodes()[0];
            bool foundConnected{ false };
            for (unsigned j = 1; j < size; j++) {
                vector<string> nodesVisited;
                if (DFS_inside(access.first, partition.getAccessNodes()[j], red, partition, nodesVisited) == IS_CONNECTED) {
                    access.second = partition.getAccessNodes()[j];
                    foundConnected = true;
                    eraseSubVector(redNodes, nodesVisited);

                    break;
                }
            }
            if (!foundConnected) {
                return (false);
            }
            //verifica no blue se os mesmos pontos de entrada e saida funcionam
            {
                vector<string> nodesVisited;
                if (DFS_inside(access.first, access.second, blue, partition, nodesVisited) == IS_CONNECTED) {
                    eraseSubVector(blueNodes, nodesVisited);
                } else {
                    //a entrada e saida encontrada no red não funciona no blue também
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

vector<int> GPX2::choose(cityMap red, cityMap blue, partitionMap allPartitions)
{
    vector<int> partitionsChoosen;
    for (auto p : allPartitions) {
        int index{ 0 };
        double totalRed{ 0.0 }, totalBlue{ 0.0 };
        vector<string> accessVec = p.second.getAccessNodes();
        for (unsigned i = 0; i < accessVec.size() / 2; i++) {
            totalRed = parcialDistance(accessVec.at(index), accessVec.at(i + 1), red, p.second);
            totalBlue = parcialDistance(accessVec.at(index), accessVec.at(i + 1), blue, p.second);
            index++;
        }
        if (totalRed < totalBlue) {
            partitionsChoosen.push_back(RED);
        } else {
            partitionsChoosen.push_back(BLUE);
        }
    }
    return partitionsChoosen;
}

// -----------------------------------------------------------------------------

// STEP 8 - Gerar o mapa do filho

void GPX2::buildOffspring(vector<int>& partitionsChoose, partitionMap& allPartitions, cityMap& red, cityMap& blue)
{
    cityMap offspring = red; // red é o pivo.
    int index{ 0 };

    for (auto& allP : allPartitions) {

        if (partitionsChoose[index] == BLUE) { // se o Blue for melhor que o Red naquela partição
            for (string s : allP.second.getNodes()) {
                delete red.at(s);
                red.erase(s);

                CityNode* newNode = new CityNode(blue.at(s)->getId(), blue.at(s)->getX(), blue.at(s)->getY());
                newNode->setEdges(blue.at(s)->getEdges());
                red.insert(make_pair(s, newNode));
            }
        }
        index++;
    }
}

void GPX2::removeGhosts(cityMap& graph)
{
    for (auto node : graph) {
        string token{ "-" };
        int index = node.first.find(token);
        if (index != string::npos) {
            //pegar o id do nó sem o token de ghost
            string id = node.first;
            id.erase(index, token.size());

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

Tour GPX2::mapToTour(cityMap& mapOffspring)
{
    Tour offspring;
    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;

    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    CityNode* city = mapOffspring.begin()->second; // Reduzir chamadas (CityNode dentro do map)

    isAlreadyVisited.push_back(mapOffspring.begin()->first);

    offspring.getTour().push_back(City(stoi(city->getId()), city->getX(), city->getY())); // já foi visitado então entra no filho

    nextToVisit.push_back(city->getEdges()[0].first);

    while (!nextToVisit.empty()) {

        isAlreadyVisited.push_back(nextToVisit.front());
        city = mapOffspring[nextToVisit.front()];
        nextToVisit.pop_front();

        offspring.getTour().push_back(City(stoi(city->getId()), city->getX(), city->getY()));

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

void GPX2::deleteMap(cityMap& m)
{ // deletar o mapa completamente,
    // desalocando os ponteiros tb

    for (auto& it : m) {
        delete it.second;
        it.second = nullptr;
    }
    m.clear();
}

int GPX2::DFS_outside(string id, cityMap unitedGraph, partitionMap allPartitions)
{
    string now;
    int idPartition = whichPartition(id, allPartitions);
    vector<string> partition = allPartitions[idPartition].getNodes();
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
    partitionConnected = whichPartition(alreadyVisited.back(), allPartitions);

    if (idPartition == partitionConnected) {
        //se conectar em si mesmo, seta o nó de entrada e o ultimo onde chegou como não access
        unitedGraph[id]->setAccess(false);
        unitedGraph[alreadyVisited.back()]->setAccess(false);
        return CONNECTED_TO_SELF;
    } else {
        //partitions[idPartition].getConnectedTo().push_back(partitionConnected);
        return CONNECTED_TO_PARTITION;
    }
}

int GPX2::DFS_inside(string entry, string exit, cityMap father, Partition partitionObj, vector<string>& returnVector)
{
    //fazer uma busca em profundidade dentro da partição
    string now;
    vector<string> alreadyVisited;
    deque<string> nextToVisit;
    vector<string> partition = partitionObj.getNodes();
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
    return (alreadyVisited.back() == exit ? IS_CONNECTED : IS_NOT_CONNECTED);
}

double GPX2::distance(double x1, double y1, double x2, double y2)
{
    return (sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2)));
}

void GPX2::eraseSubVector(vector<string>& vec, vector<string>& subvec)
{
    for (unsigned i = 0; i < subvec.size(); i++) {
        for (unsigned j = 0; vec.size(); j++) {
            if (subvec[i].compare(vec[j]) == 0) {
                vec.erase(vec.begin() + j);
                break;
            }
        }
    }
}

double GPX2::parcialDistance(string entry, string exit, cityMap father, Partition partitionObj)
{
    //fazer uma busca em profundidade dentro da partição
    string now;
    vector<string> alreadyVisited;
    deque<string> nextToVisit;
    vector<string> partition = partitionObj.getNodes();
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

void GPX2::printMap(cityMap& m)
{
    for (map<string, CityNode*>::iterator it = m.begin(); it != m.end(); it++) {
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
    }
}

int GPX2::whichPartition(const string id, partitionMap allPartitions)
{ // Procura em qual partição está a cidade procurada, retorna o ID da partição
    for (auto& p : allPartitions) {
        if (find(p.second.getNodes().begin(), p.second.getNodes().end(), id) != p.second.getNodes().end()) {
            return (p.first);
        }
    }
    return (-1);
}
