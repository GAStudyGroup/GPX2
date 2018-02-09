#include "CityMap.hpp"

CityMap tourToMap(Tour& t)
{ // Mapear o tour para um grafo com ligações entre os nós

    if (t.getRoute().empty()) {
        exit(EXIT_FAILURE);
    }

    map<string, CityNode*> aux; // Mapa com as conexões dos nodes 
    double dist = 0;
    vector<City> cities{ t.getRoute() }; 

    CityNode* prev = new CityNode(to_string(cities[0].getId()), cities[0].getX(), cities[0].getY()); // ponto anterior ao atual dentro do for
    CityNode* first = prev; // referência do primeiro acesso

    aux.insert(make_pair(first->getId(), first)); // gera o mapa e insere o primeiro dentro dele

    for (unsigned i = 1; i < t.getRoute().size(); i++) { // percorre o vetor a partir do segundo elemento, o primeiro já foi transformado
        CityNode* cn = new CityNode(to_string(cities[i].getId()), cities[i].getX(), cities[i].getY()); // gera um node com o segundo elemento

        aux.insert(make_pair(cn->getId(), cn)); // insere o node dentro do mapa

        dist = distance(*prev, *cn);

        cn->addEdge(CityNode::node(prev->getId(), dist)); // adiciona ao node atual as arestas de conexão

        dist = distance(*cn, *prev);
        prev->addEdge(CityNode::node(cn->getId(), dist)); // adiciona ao node anterior o atual como um próx (lista duplamente encadeada)

        prev = cn; // o anterior recebe o atual para continuar o for
    }

    dist = distance(*prev, *first);
    first->addEdge(CityNode::node(prev->getId(), dist)); // o primeiro recebe o atual ao sair do for, completando os ligamentos das arestas

    dist = distance(*first, *prev);
    prev->addEdge(CityNode::node(first->getId(), dist)); // o atual recebe o primeiro para completar os ligamentos

    return (aux); // retorna o mpaa com os nodes já instanciados e adicionados
}

Tour mapToTour(CityMap& mapOffspring)
{ // Map para tour
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

double distance(const CityNode& c1, const CityNode& c2)
{
    return (round(sqrt(pow((c1.getX() - c2.getX()), 2) + pow((c1.getY() - c2.getY()), 2))));
}

void printMap(CityMap& graph, std::ostream& stream)
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

void deleteCityMap(CityMap& m)
{ // deletar o mapa completamente,
    // desalocando os ponteiros tb

    for (auto& it : m) {
        delete it.second;
        it.second = nullptr;
    }
    m.clear();
}