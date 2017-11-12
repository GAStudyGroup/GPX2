#include "GPX2.hpp"

Tour GPX2::crossover(Tour redT, Tour blueT)
{
    cout<<"in GPX2"<<endl;
    GPX2 obj;
    // Step 1
    obj.red = obj.tourToMap(redT);
    obj.blue = obj.tourToMap(blueT);

    std::ofstream file;
    file.open("LOG_DO_HELL.log");

    printMap(obj.red,file);
    printMap(obj.blue,file);
    file.close();
    // Step 2
    obj.createGhosts();


    // Step 3 
    obj.joinGraphs();


    // Step 4
    obj.cutCommonEdges();


    // Step 5
    obj.findAllPartitions();
    obj.cleanInsideAccess();

    if (obj.allPartitions.size() < 2) {
        obj.deleteAll();
        return ((redT.getFitness() > blueT.getFitness()) ? redT : blueT);
    }


    // Step 6
    obj.checkAllPartitions();

    std::ofstream file3;
    file3.open("LOG_DO_HELL_PARTITIONS_BEFORE_FUSION.log");
    file3<<"size "<<obj.allPartitions.size()<<endl;
    for(auto p : obj.allPartitions){
        file3<<(*p.second)<<endl;
    }
    
    file3.close();

    // Fusion
    obj.fusion();

    std::ofstream file2;
    file2.open("LOG_DO_HELL_PARTITIONS.log");
    file2<<"size "<<obj.allPartitions.size()<<endl;
    for(auto p : obj.allPartitions){
        file2<<(*p.second)<<endl;
    }
    
    file2.close();

    // Step 7
    obj.choose();

    /* cout<<"choosen partitions "<<endl;
    for(auto p : obj.partitionsChoosen){
        cout<<((p==Parent::RED)?("RED"):("BLUE"))<<endl;
    } */

    /* cout<<"Red fitness before "<<obj.totalDistance(obj.red)<<"\n";
    cout<<"Blue fitness before "<<obj.totalDistance(obj.blue)<<"\n"; */

    // Step 8
    obj.buildOffspring();
    

    /* cout<<"Red fitness after "<<obj.totalDistance(obj.red)<<"\n";
    cout<<"Blue fitness after "<<obj.totalDistance(obj.blue)<<"\n"; */

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

    // Deletar as coisas

    cout<<"out GPX2"<<endl;
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
                isGhost.insert(city.second->getEdges().at(i).first);
                isGhost.insert(blue.at(idKey)->getEdges().at(i).first);
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
    CityNode::node edgeFirst = tour.at(id)->getEdges().at(0); // edge que será mandado para o ghost
    tour[id]->deleteEdge(0); // delete edge
    tour[id]->addEdge(CityNode::node(ghost->getId(), 0)); // adiciona ao node "REAL" o edge de ligação com o ghost

    for (unsigned i = 0; i < 2; i++) {
        // percorre os edges de ligação com o nó que foi para o ghost
        CityNode::node edge = tour.at(edgeFirst.first)->getEdges().at(i);

        if (edge.first.compare(id) == 0) { // verifica se é a ligação

            // adiciona a ligação ao ghost e deleta do "REAL"
            tour.at(edgeFirst.first)->deleteEdge(i);
            tour.at(edgeFirst.first)->addEdge(CityNode::node(ghost->getId(), edgeFirst.second));
        }
    }

    ghost->addEdge(edgeFirst); // faz o ghost se ligar ao edge
    ghost->setAccess(true);
    ghost->addEdge(CityNode::node(tour.at(id)->getId(), 0)); // adiciona o REAL como um edge do ghost

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
{   // Irá retirar as flags Access dos vértices que se ligam internos a partição

    // Percorre a partição
    for (auto& p : allPartitions) {
        vector<string> tmp;
        
        // Percorre os vértices que estão na partição
        for (string id : p.second->getAccessNodes()) {

            // Verificar se o vértice está ligado a outras partições
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
                // Insere o caminho da ligação na partição
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
    /* for (auto p : allPartitions) {
        //se não for uma partição recombinante ele deleta da lista de partições
        if (!checkPartition(p.second)) {
            unfeasiblePartitions.insert(make_pair(p.first, p.second));
            allPartitions.erase(p.first);
        }
    } */

    for (auto it=allPartitions.begin();it!=allPartitions.end();) {
        //se não for uma partição recombinante ele deleta da lista de partições
        if (!checkPartition((*it).second)) {
            unfeasiblePartitions.insert(make_pair((*it).first,(*it).second));
            it = allPartitions.erase(it);
        }else{
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
        //não é uma partição recombinante pois não possui uma entrada para cada saida.
        return (false);
    } else {
        vector<string> nodesInPartition = partition->getNodes();
        
        vector<string> accessVec = partition->getAccessNodes();
        pair<string, string> access;
        set<pair<string,string>,cmp2> entryAndExit;
        bool foundConnected{ false };
        for (unsigned i = 0; i < size; i++) {
            //encontrando entrada e saida no red e verifica se elas existem
            for (unsigned j = 0; j < size; j++) {
                if (i != j) {
                    pair<SearchResult,vector<string>> resultRed = DFS_inside(accessVec.at(i),accessVec.at(j),red,partition);

                    pair<SearchResult,vector<string>> resultBlue = DFS_inside(accessVec.at(i),accessVec.at(j),blue,partition);

                    if ((resultRed.first == SearchResult::IS_CONNECTED) && (resultBlue.first == SearchResult::IS_CONNECTED)) {
                        foundConnected = true;
                        eraseSubVector(redNodes, resultRed.second);
                        eraseSubVector(blueNodes, resultBlue.second);
                        break;
                    }
                }
            }
            //se não possui nenhuma saida e entrada conectada
            if (!foundConnected) {
                return (false);
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

void GPX2::fusion()
{   // função que irá executar a Fusion do GPX2, irá verificar as partições unfeasible e tentar "transformá-las" em feasible

    bool atLeastOneConnected{ false };

    // Continua a execução enquanto existir mais de uma partição para tentar realizar a fusão e elas possuirem conexões com outras
    while ((unfeasiblePartitions.size() > 1)) {

        /* for(auto uP : unfeasiblePartitions){
            cout<<*uP.second<<endl;
        } */

        //verifica quais partições estão conectadas
        atLeastOneConnected = unfeasiblePartitionsConnected();
        if (!atLeastOneConnected) {
            break;
        }

        // Conta o total de vezes que cada partição se conecta com outra unfeasible, ou seja, os nós que possuem um "caminho" de uma partição à outra
        countConnectedPartitions(); 

        // Executa a fusão das partições de acordo com as verificações feitas

        /* for(auto uP : unfeasiblePartitions){
            cout<<*uP.second<<endl;
        } */

        fusePartitions();

        //clear aux
        for(auto part : unfeasiblePartitions){
            part.second->getConnectedTo().clear();
            part.second->getConnections().clear();
        }

        // Irá verificar se a fusão gerou uma partição feasible, caso aconteça ela será colocada nas partições feasible (allPartitions)
        checkUnfeasiblePartitions();
    }
}

// -----------------------------------------------------------------------------

// STEP 7 - Escolher partições para o filho

void GPX2::choose()
{
    //encontrar as entradas e saídas
    for(auto p : allPartitions){

        int totalRed{0},totalBlue{0};

        set<pair<string,string>,cmp2> entryAndExit;
        vector<string> accessVec = p.second->getAccessNodes();
        for(unsigned i=0;i<accessVec.size();i++){
            for(unsigned j=0;j<accessVec.size();j++){
                if(i != j){
                    pair<SearchResult,vector<string>> result = DFS_inside(accessVec.at(i),accessVec.at(j),red,p.second);
                    pair<SearchResult,vector<string>> resultTmp = DFS_inside(accessVec.at(i),accessVec.at(j),blue,p.second);
                    if(result.first==SearchResult::IS_CONNECTED){
                        entryAndExit.insert(make_pair(accessVec.at(i),accessVec.at(j)));
                        /* if(res.second){
                            totalRed+=parcialDistance(accessVec.at(i),accessVec.at(j),red,p.second);
                            totalBlue+=parcialDistance(accessVec.at(i),accessVec.at(j),blue,p.second);
                            
                            cout<<"blue ok? "<<(resultTmp.first==SearchResult::IS_CONNECTED)<<endl;
                            
                            cout<<"totalRed parcial "<<totalRed<<endl;
                            cout<<"totalBlue parcial "<<totalBlue<<endl;
                        } */
                        
                    }
                }
            }
        }
                        /* cout<<"totalRed "<<totalRed<<endl;
                        cout<<"totalBlue "<<totalBlue<<endl; */
        totalRed=0;totalBlue=0;
        //int totalRed{0},totalBlue{0};
        for(auto pair : entryAndExit){
            totalRed+=parcialDistance(pair.first,pair.second,red,p.second);
            totalBlue+=parcialDistance(pair.first,pair.second,blue,p.second);
        }
        /* cout<<"RED: "<<totalRed<<" BLUE: "<<totalBlue<<" choosen: "<<((totalRed < totalBlue)?("RED"):("BLUE"))<<endl; */
        cout<<"RED: "<<totalRed<<" OR BLUE: "<<totalBlue<<"?"<<endl;
        if (totalRed < totalBlue) {
            cout<<"partition "<<p.first<<" RED"<<endl;
            partitionsChoosen.push_back(Parent::RED);
        } else {
            cout<<"partition "<<p.first<<" BLUE"<<endl;
            partitionsChoosen.push_back(Parent::BLUE);
        }
    }
}

// -----------------------------------------------------------------------------

// STEP 8 - Gerar o mapa do filho

void GPX2::buildOffspring()
{
    int index{ 0 };

    cout<<"feasible partitions "<<endl;
    for(auto p : allPartitions){
        cout<<p.first<<" ";
    }
    cout<<endl;
    cout<<" BEFORE CHANGE RED: "<<totalDistance(red)<<" BLUE: "<<totalDistance(blue)<<endl;
    for (auto& allP : allPartitions) {
        if (partitionsChoosen[index] == Parent::BLUE) { // se o Blue for melhor que o Red naquela partição
            cout<<"partition "<<allP.first<<" BLUE"<<endl;
            for (string s : allP.second->getNodes()) {
                delete red.at(s);
                red.erase(s);

                CityNode* newNode = new CityNode(blue.at(s)->getId(), blue.at(s)->getX(), blue.at(s)->getY());
                newNode->setEdges(blue.at(s)->getEdges());
                red.insert(make_pair(s, newNode));
            }
        } else {
            cout<<"partition "<<allP.first<<" RED"<<endl;
            for (string s : allP.second->getNodes()) {
                delete blue.at(s);
                blue.erase(s);

                CityNode* newNode = new CityNode(red.at(s)->getId(), red.at(s)->getX(), red.at(s)->getY());
                newNode->setEdges(red.at(s)->getEdges());
                blue.insert(make_pair(s, newNode));
            }
        }
        cout<<" AFTER CHANGE "<<index<<" RED: "<<totalDistance(red)<<" BLUE: "<<totalDistance(blue)<<endl;
        index++;
    }
    offspringChoosen = ((totalDistance(red) < totalDistance(blue)) ? (Parent::RED) : (Parent::BLUE));
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
{   // Map para tour
    
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

         // Cria um objeto e carrega no Tour filho
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
pair<GPX2::SearchResult,vector<string>> GPX2::DFS_inside(string entry, string exit, CityMap father, Partition* partitionPtr)
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

    return (make_pair((alreadyVisited.back() == exit ? SearchResult::IS_CONNECTED : SearchResult::IS_NOT_CONNECTED),alreadyVisited));
}

double GPX2::distance(double x1, double y1, double x2, double y2)
{
    return (round(sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2))));
}

void GPX2::eraseSubVector(vector<string>& vec, vector<string>& subvec)
{   // Apaga um subvetor
    for (unsigned i = 0; i < subvec.size(); i++) {
        for (unsigned j = 0; j < vec.size(); j++) {
            if (subvec[i].compare(vec[j]) == 0) {
                vec.erase(vec.begin() + j);
                break;
            }
        }
    }
}

int GPX2::parcialDistance(string entry, string exit, CityMap father, Partition* partitionPtr)
{   // Distancia do SubTour da partição no pai

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

    return(totalDistance);
}

void GPX2::printMap(CityMap &graph,std::ostream &stream)
{
    stream<<"size: "<<graph.size()<<endl;
    for (map<string, CityNode*>::iterator it = graph.begin(); it != graph.end(); it++) {
        stream << " " << it->first << " | " << graph[it->first]->getId() << endl;
        if(it->second->getAccess()){
            stream << "Access: " << true << endl;
        }else{
            stream << "Access: " << false << endl;
        }
        stream << "==================================" << endl;

        stream<<"connections: "<<it->second->getEdges().size()<<endl;
        for (unsigned i = 0; i < it->second->getEdges().size(); i++) {
            stream << "Edge " << i << ": " << graph[it->second->getEdges().at(i).first]->getId();
            stream << "\t#\t";
            stream << "Distance: " << it->second->getEdges().at(i).second << endl;
        }
        stream << "----------------------------------" << endl;
        stream << endl;
    }

    stream<<"______________________________________________________________THE DIVISION______________________________________________________________"<<endl;

    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    bool notAlreadyVisited{ false };
    bool notToVisit{ false };

    CityNode* city = graph.begin()->second; // Reduzir chamadas (CityNode dentro do map)

    isAlreadyVisited.push_back(graph.begin()->first);
    stream<<graph.begin()->first<<endl;
    nextToVisit.push_back(city->getEdges()[0].first);

    while (!nextToVisit.empty()) {

        stream<<nextToVisit.front()<<endl;
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
    stream<<"size "<<isAlreadyVisited.size()<<endl;
    stream<<"map size "<<graph.size()<<endl;
}

int GPX2::totalDistance(CityMap& graph)
{   // Distancia total para percorrer o grafo

    deque<string> nextToVisit;
    vector<string> isAlreadyVisited;
    int totalDistance{0};
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

/*  
    -------------------------------------------------------------------------------------
    Fusion utils
*/

void GPX2::checkUnfeasiblePartitions()
{   // Função que irá verificar se foi gerada alguma partição feasible durante a fusão
    for (auto it=unfeasiblePartitions.begin(); it!=unfeasiblePartitions.end();) {
        if (checkPartition((*it).second)) { // Checar se é feasible
            // se for irá ser retirada das partições unfeasible e colocada nas partições feasible
            allPartitions.insert(make_pair((*it).first,(*it).second));
            it = unfeasiblePartitions.erase(it);
        }else{
            it++;
        }
    }
}

void GPX2::countConnectedPartitions()
{   // Função que irá realizar a contagem das ligações entre as partições

    for (auto& p : unfeasiblePartitions) {
        map<int, int> countConnected;
        for (auto node : p.second->getConnectedTo()) {

            // Tenta inserir uma nova partição que está conectada
            auto result = countConnected.insert(make_pair(node.first, 1));

            // Caso a inserção falhar, a partição já está na lista de conectadas, sendo assim o total de ligações será aumentado
            if (!result.second) {
                countConnected.at(node.first)++;
            }
        }

        // Insere os resultados da contagem
        for (auto node : countConnected) {
            p.second->getConnections().push_back(make_pair(node.first, node.second));
        }
    }
}

void GPX2::fusePartitions()
{   // Realiza a fusão entre as partições, de acordo com as validações já realizadas

    vector<int> partitionsToCheck; 
    set<unfeasibleConnection, cmp> fuseWith;
    vector<int> numberOfConnections;

    // Percorre as partições gerando um par de fusões
    for (auto p : unfeasiblePartitions) {
        
        pair<pair<int, int>,int> data = whichPartitionToFuseWith(unfeasiblePartitions.at(p.first));
        // Se for "-1" então ela está conectada à uma partição feasible

        if (data.first.first != -1) {
            // Carrega o par de partições para fundir
            fuseWith.insert(make_pair(make_pair(p.first, data.first.first),data.second));
            // Carrega o valor de conexões entre elas
            numberOfConnections.push_back(data.first.second);
        }
    }

    for(const auto &p : unfeasiblePartitions){
        vector<unfeasibleConnection> tmp = fusionsWithPartition(p.first, fuseWith);
        if(fuseWith.size()<2){
            break;
        }
        if(!tmp.empty()){
            int maxCon{-1}, maxPos{-1};
            for(unsigned i=0; i<tmp.size();i++){
                if(tmp[i].second > maxCon){
                    maxCon = tmp[i].second;
                    maxPos = i;
                }
            }
            
            
            tmp.erase(tmp.begin() + maxPos);
            
            for(const auto &unfCon : tmp){
                for(auto it=fuseWith.begin();it!=fuseWith.end();){

                    if(unfCon==(*it)){
                        
                        it = fuseWith.erase(it);
                    }else{
                        it++;
                    }
                }
            }
        }
        
    }

    // Início da execução da fusão em si
    for (auto p : fuseWith) {
        // Não precisa mudar o custo pois a checkPartition não verifica isso
        Partition* p1Ptr = unfeasiblePartitions.at(p.first.first);
        Partition* p2Ptr = unfeasiblePartitions.at(p.first.second);
        vector<string> intermediariesNodes, accessNodesToRemove;
        
        // Deletar as entradas de conexões (connectTo) diferentes da partição que será fundida
        for (auto it = p1Ptr->getConnectedTo().begin(); it != p1Ptr->getConnectedTo().end();) {
            if ((*it).first != p.first.second) {
                p1Ptr->getConnectedTo().erase(it);
            } else {
                it++;
            }
        }
        for (auto it = p2Ptr->getConnectedTo().begin(); it != p2Ptr->getConnectedTo().end();) {
            if ((*it).first != p.first.first) {
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

        // Percorrer os nós com ligação na partição que será feita a fusão
        for (auto connectedNode : p2Ptr->getConnectedTo()) {

            // Pegar os nós intermediários (o "caminho" da conexão)
            pair<SearchResult, vector<string>> tmp = DFS_outside(connectedNode.second.first, unfeasiblePartitions);

            // Apagar os access nodes da lista da partição
            accessNodesToRemove.push_back(connectedNode.second.first);
            accessNodesToRemove.push_back(connectedNode.second.second);
            
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

        // Coma partição gerada, os antigos AcessNodes não são mais AccesNodes, então serão apagados
        eraseSubVector(p1Ptr->getAccessNodes(), accessNodesToRemove);

        // Apagar tudo
        delete p2Ptr;
        unfeasiblePartitions.erase(p.first.second);
    }
}

vector<GPX2::unfeasibleConnection> GPX2::fusionsWithPartition(const int id, set<GPX2::unfeasibleConnection, cmp> &connections){
    vector<GPX2::unfeasibleConnection> instances;

    for(const auto &con : connections){
        if(con.first.first == id || con.first.second == id){
            instances.push_back(con);
        }
    }
    return(instances);    
}

bool GPX2::unfeasiblePartitionsConnected()
{   // Verifica as conexões das partições unfeasible

    vector<string> nodesToCheck;
    bool atLeastOneConnected{false};

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
        int connectedPartition = unfeasiblePartitions.at(idPartition)->getConnectedTo().back().first;

        // Verifica o ID do nó na qual ela está conectada
        string connectedId = unfeasiblePartitions.at(idPartition)->getConnectedTo().back().second.second;

        // Caso ele esteja conectado a outra partição unfeasible, caso não seja "-1" (conectado com uma partição feasible)
        if (connectedPartition != -1) {
            atLeastOneConnected = true;

            // Insere no vetor de conexões
            unfeasiblePartitions.at(connectedPartition)->getConnectedTo().push_back(make_pair(idPartition, make_pair(connectedId, (*it))));

            // Não precisa verificar os nós que já foram ligados
            nodesToCheck.erase(remove(nodesToCheck.begin(), nodesToCheck.end(), connectedId), nodesToCheck.end());
        } else {
            // Apaga por estar conectado com uma feasible
            unfeasiblePartitions.at(idPartition)->getConnectedTo().erase(unfeasiblePartitions.at(idPartition)->getConnectedTo().end());
        }
    }
    return (atLeastOneConnected);
}
 
GPX2::unfeasibleConnection GPX2::whichPartitionToFuseWith(Partition* partition)
{   // Retorna um par com a partição que contém mais conexões e o total de conexões

    int partitionId{ -1 }, max{ -1 };
    for (auto p : partition->getConnections()) {
        if (p.second > max) {
            max = p.second;
            partitionId = p.first;
        }
    }
    return (make_pair(make_pair(partitionId, max),max));
}

bool operator==(GPX2::unfeasibleConnection &uf1, GPX2::unfeasibleConnection &uf2){
    bool first = ((uf1.first.first == uf2.first.first) || (uf1.first.first == uf2.first.second));
    bool second = ((uf1.first.second == uf2.first.first) || (uf1.first.second == uf2.first.second));

    return(first && second);
}