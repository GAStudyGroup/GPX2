#ifndef GPX2_HPP
#define GPX2_HPP

#include <algorithm>
#include <deque>
#include <iterator>
#include <map>
#include <set>
#include <stack>
#include <string>

#include "City.hpp"
#include "CityNode.hpp"
#include "ListOfCities.hpp"
#include "Partition.hpp"

using std::string;
using std::to_string;
using std::stack;
using std::map;
using std::make_pair;
using std::find;
using std::deque;
using std::set;
using std::size_t;
using std::remove;

class GPX2 {

    const string ghostToken = "-";

    using CityMap = map<string, CityNode*>;
    using PartitionMap = map<int, Partition*>;

    enum class SearchResult { CONNECTED_TO_PARTITION,
        CONNECTED_TO_SELF,
        IS_CONNECTED,
        IS_NOT_CONNECTED };
    enum class Parent { RED,
        BLUE };

public:
    // Método principal do funcionamento do crossover, recebe dois pais e retorna um Tour filho
    Tour static crossover(Tour, Tour);

private:
    // Step 1 - Mapear o Tour passado, irá retornar um grafo com as conexões mapeadas
    CityMap tourToMap(Tour&);

    // Step 2 - Irá criar os nós GHOSTS e inseri-los nos grafos dos pais
    // executar essa ação antes das outras irá "cortar" gasto computacional repetitivo
    void createGhosts();
    void insertGhost(string&, CityMap&, CityNode*);

    // Step 3 - Irá gerar o GU (Gráfico da União) dos grafos dos pais
    void joinGraphs();

    // Step 4 - Irá gerar o GU', o grafo da união dos pais com os cortes executados
    void cutCommonEdges();

    // Step 5 - Irá encontrar as partições componentes do GU'
    vector<string> findPartition(const string);
    void findAllPartitions();
    void cleanInsideAccess();

    // Step 6 - Irá checar se as partições encontradas são recombinantes
    void checkAllPartitions();
    bool checkPartition(Partition*);

    // Step 7 - Escolher partições para o filho
    void choose();

    // Step 8 - Irá gerar o mapa do filho e depois remover os ghosts
    void buildOffspring();
    void removeGhosts(CityMap&);

    // Step 9 - Irá linearizar o mapa do filho para formato Tour
    Tour mapToTour(CityMap&);

    // Utilities
    vector<string> cityToString(vector<City>);
    void deleteAll();
    void static deleteCityMap(CityMap&);
    void static deletePartitionMap(PartitionMap&);
    SearchResult DFS_outside(string, PartitionMap, bool = false);
    SearchResult DFS_inside(string, string, CityMap, Partition*, vector<string>&);
    double distance(double, double, double, double);
    void eraseSubVector(vector<string>&, vector<string>&);
    double parcialDistance(string, string, CityMap, Partition*);
    void printMap(CityMap&);
    double totalDistance(CityMap&);
    int whichPartition(const string, PartitionMap);

    /* 
    NEW FUNCTIONS
    */
    bool unfeasiblePartitionsConnected();
    void countConnectedPartitions();
    int whichPartitionToFuseWith(Partition*);
    void fusePartitions();
    vector<string> DFS_outside_get_nodes(string, PartitionMap);
    void fusion();
    void checkUnfeasiblePartitions();
    /* 
    VARS
    */
    PartitionMap allPartitions;
    PartitionMap unfeasiblePartitions;
    CityMap unitedGraph;

    CityMap red;
    CityMap blue;
    vector<Parent> partitionsChoosen;
    Parent offspringChoosen;
};

#endif