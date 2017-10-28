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

class GPX2 {

    typedef map<string, CityNode*> cityMap;
    //mudar para ponteiro depois
    typedef map<int, Partition> partitionMap;

    enum searchResult { CONNECTED_TO_PARTITION,
        CONNECTED_TO_SELF,
        IS_CONNECTED,
        IS_NOT_CONNECTED };
    enum parent { RED,
        BLUE };

public:
    // Método principal do funcionamento do crossover, recebe dois pais e retorna um Tour filho
    Tour crossover(Tour, Tour);

private:
    // Step 1 - Mapear o Tour passado, irá retornar um grafo com as conexões mapeadas
    cityMap tourToMap(Tour&);

    // Step 2 - Irá criar os nós GHOSTS e inseri-los nos grafos dos pais
    // executar essa ação antes das outras irá "cortar" gasto computacional repetitivo
    void createGhosts(cityMap&, cityMap&);
    void insertGhost(string&, cityMap&, CityNode*);

    // Step 3 - Irá gerar o GU (Gráfico da União) dos grafos dos pais
    void joinGraphs(cityMap&, cityMap&, cityMap&);

    // Step 4 - Irá gerar o GU', o grafo da união dos pais com os cortes executados
    void cutCommonEdges(cityMap&);

    // Step 5 - Irá encontrar as partições componentes do GU'
    vector<string> findPartition(const string, cityMap);
    void findAllPartitions(cityMap, partitionMap&);
    void cleanInsideAccess(cityMap, partitionMap&);

    // Step 6 - Irá checar se as partições encontradas são recombinantes
    bool checkPartition(cityMap, cityMap, cityMap&, Partition&);
    void checkAllPartitions(cityMap, cityMap, cityMap&, partitionMap&);

    // Step 7 - Escolher partições para o filho
    vector<int> choose(cityMap, cityMap, partitionMap);

    // Step 8 - Irá gerar o mapa do filho e depois remover os ghosts
    void buildOffspring(vector<int>&, partitionMap&, cityMap&, cityMap&);
    void removeGhosts(cityMap&);

    // Step 9 - Irá linearizar o mapa do filho para formato Tour
    Tour mapToTour(cityMap&);

    // Utilities
    vector<string> cityToString(vector<City>);
    void deleteMap(cityMap&);
    int DFS_outside(string, cityMap, partitionMap);
    int DFS_inside(string, string, cityMap, Partition, vector<string>&);
    double distance(double, double, double, double);
    void eraseSubVector(vector<string>&, vector<string>&);
    double parcialDistance(string, string, cityMap, Partition);
    void printMap(cityMap&);
    int whichPartition(const string, partitionMap);
};

#endif