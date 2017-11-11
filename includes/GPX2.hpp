#ifndef GPX2_HPP
#define GPX2_HPP

#include <algorithm>
#include <deque>
#include <iterator>
#include <map>
#include <set>
#include <stack>
#include <string>

#include <fstream>

#include "City.hpp"
#include "CityNode.hpp"
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

    // Definições das principais estruturas utilizadas
    const string ghostToken = "-";

    using CityMap = map<string, CityNode*>;
    using PartitionMap = map<int, Partition*>;

    enum class SearchResult { CONNECTED_TO_PARTITION,
        CONNECTED_TO_SELF,
        IS_CONNECTED,
        IS_NOT_CONNECTED };
    enum class Parent { RED,
        BLUE };

    struct cmp
    {
        bool operator()(const pair<pair<int, int>,int> &p1, const pair<pair<int, int>,int> &p2){
            bool first = (p1.first.first==p2.first.first || p1.first.first==p2.first.second);
            bool second = (p1.first.second==p2.first.first || p1.first.second==p2.first.second);
            return(!(first && second));
        }
    };
    struct cmp2
    {
        bool operator()(const pair<string, string> &p1, const pair<string, string> &p2){
            bool first = (!(p1.first.compare(p2.first)) || !(p1.first.compare(p2.second)));
            bool second = (!(p1.second.compare(p2.first)) || !(p1.second.compare(p2.second)));
            return(!(first && second));
        }
    };
    // -------------------------------------------------

public:
    using unfeasibleConnection = pair<pair<int, int>,int>;
    
    /*  
        Método principal do GPX.
        Recebe duas estruturas Tour, que serão os pais, e retorna um novo Tour que será o filho gerado.
        Todos os passos executados pelo GPX são executados durante sua execução.
    */
    Tour static crossover(Tour, Tour);

private:
    /*  
        STEP 1 - Mapeamento do Tour

        A estrutura Tour, inicialmente, é uma lista de cidades no qual a ordem que elas estão dispostas representa a ordem de visitação.
        Para a realização do GPX é necessário que o Tour esteja disposto na forma de um grafo, com vértices sendo as cidades e arestas as ligações entre elas.

        O método recebe o Tour como parâmetro e gera um Grafo do mesmo.
    */
    CityMap tourToMap(Tour&);


    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 2 - Adição dos GhostNodes

        O GPX implementa a ideia de GhostNode, quando os dois grafos pais são unidos podem existir nós que possuam grau 4 (ligações com outros 4 vértices). 
        Esses vertices podem ser duplicados, gerando um vértice fantasma no mesmo ponto do vértice real, com isso é possível aumentar o total de partições encontradas.

        Teóricamente os GhostNodes são adicionados ao grafo após a união dos grafos dos pais, porém para cortar custos computacionais eles são executados antes da união, sendo assim quando os grafos são unidos os GhostNodes já são inseridos no grafo da união.
    */
    // O método irá verificar os nós que irão se tornar Ghosts e será criado o nó Ghost
    void createGhosts();
    // O método irá inserir o nó Ghost criado no Grafo do pai
    void insertGhost(string&, CityMap&, CityNode*);


    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 3 - União dos Grafos dos pais

        Para a criação das partições é necessário que um Grafo com a união dos pais seja gerado (GU), como se os dois pais fossem sobrepostos.
        
        O método irá unir o Tour Red com o Tour Blue
    */
    void joinGraphs();


    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 4 - Cortar as arestas sobrepostas

        Após a criação do GU é necessário retirar as arestas sobrepostas, ou seja, as arestas que estão nos dois pais. Com esse corte as partições "começam a aparecer".
        Os cortes são simbolizados com arestas de custo 0 de ligação e são marcados como AccessNodes.

        O método realiza os cortes, gerando o GU'.
    */
    void cutCommonEdges();


    // -----------------------------------------------------------------------------------------------------
    /* 
        STEP 5 - Encontrar as partições
        
        Para a execução do GPX, após a criação do GU' é preciso encontrar "SubTours" que simbolizem partições, essas partições serão utilizadas na formação do filho posteriormente.
        As partições são circuitos com ligações entre os vértices desses cricuitos. Quanto maior o total de partições melhor será o desempenho do Crossover.
    */
    // O método irá encontrar a partição no qual o vértice passado está contido, retorna uma lista com todos os nós que compõem a partição.
    vector<string> findPartition(const string);
    // Irá executar a verificação anterior para todas as partições.
    void findAllPartitions();
    // Após encontradas, as partições podem conter AccessNodes ligando entre os vértices que a compõem, esses vértices deixarão de ser AccessNodes.
    void cleanInsideAccess();


    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 6 - Checar se as partições são recombinantes (feasible)

        Para poder realizar o Crossover é necessário que as partições que serão utilizadas para a criação do filho sejam recombinantes, ou seja, possuam AccessNodes de entrada e saída da partição representados em seus pais. 
        No caso, seria possível percorrer uma entrada e uma saída tanto em um pai quanto no outro.

        Se uma partição não seguir essa regra, ela se torna não-recombinantes (unfeasible). O GPX implementa uma "solução" para isso, a fim de melhorar o desempenho ele tenta fundir as partições unfeasible para torná-las feasible, caso possível.
        Se ocorrer de uma fusão gerar uma partição feasible, ela será colocada na lista de partições feasible.
    */
    // Irá verificar se a partição é feasible
    bool checkPartition(Partition*);
    // Irá executar a verificação para todas as partições encontradas
    void checkAllPartitions();
    // Para as partições unfeasible será executada a tentativa de fusion (os métodos de auxílio da fusion estão explicados abaixo)
    void fusion();

    
    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 7 - Escolher os "SubTours" que irão compor o filho

        Nesta etapa será verificado em cada pai qual possui o melhor SubTour para ser herdado pelo filho. Cada partição é composta pela união de um SubTour de cada pai, eles serão escolhidos e salvos de acordo com o melhor.

        O método irá executar a verificação em cada partição, salvando em uma lista o pai com o melhor SubTour.
    */
    void choose();


    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 8 - Criar Grafo do filho

        Após escolhidos os melhores SubTours, o mapa do filho será criado.
        Depois de criado é necessário remover os GhostNodes, eles serão unidos aos vértices de origem.

        Para melhoria, após a montagem do filho sobre os pais, é verificado qual Tour, Red ou Blue, ficou com uma fitness mais elevada, esse Tour será escolhido para ser o filho.
    */
    // O método irá montar o Grafo do filho sobre os Grafos dos dois pais, depois irá verificar qual ficou melhor
    void buildOffspring();
    // Irá remover os Ghosts do melhor filho criado
    void removeGhosts(CityMap&);


    // -----------------------------------------------------------------------------------------------------
    /*  
        STEP 9 - Linearização de Grafo para lista de City (Tour)

        Após terminado, o GPX irá retornar o Grafo filho ao estado original dos pais, forma de um Tour.
    */
    Tour mapToTour(CityMap&);

    
    /*  
    -------------------------------------------------------------------------------------------------------------

        MÉTODOS DE UTILIDADE

        Métodos utilizados para dar suporte aos passos que compõem o GPX.
    */
    // Irá transformar um vetor de Objeto Cituy em um vetor de Strings com os ID dos objeto
    vector<string> cityToString(vector<City>);
    // Métodos que irão limpar os ponteiros e vetores utilizados
    void deleteAll();
    void static deleteCityMap(CityMap&);
    void static deletePartitionMap(PartitionMap&);
    // Busca em profundidade fora da partição para encontrar conexões entre partições
    pair<SearchResult,vector<string>> DFS_outside(string, PartitionMap, bool = false);
    // Busca em profundidade dentro da partição para verificar se os AccessNodes estão conectados
    pair<SearchResult,vector<string>> DFS_inside(string, string, CityMap, Partition*);
    // Distancia entre dois pontos
    double distance(double, double, double, double);
    // Apagar subvetor de um vetor
    void eraseSubVector(vector<string>&, vector<string>&);
    // Distância parcial, usado para medir o melhor pai em cada partição
    int parcialDistance(string, string, CityMap, Partition*);
    // Imprimir o mapa
    void static printMap(CityMap&,std::ostream&);
    // Retorna a distância necessária para percorrer todo o grafo
    int totalDistance(CityMap&);
    // Retorna o ID da partição que a cidade está contida
    int whichPartition(const string, PartitionMap);

    /*  
        MÉTODOS DE UTILIDADE - FUSION

        Para execução do procedimento da fusion são necessárias várias etapas e procedimentos. 
    */
    // Verificar se todas as partições são unfeasible (caso durante uma fusão ocorra a criação de uma feasible)
    void checkUnfeasiblePartitions();
    // Contar a quantidade de conexões existentes entre as partições
    void countConnectedPartitions();
    // Fundir as partições
    void fusePartitions();
    // Retorna uma lista com todas as fusões que a partição participa
    vector<unfeasibleConnection> fusionsWithPartition(const int, set<unfeasibleConnection, cmp>&);
    // Verificar as partições unfeasible que estão conectadas
    bool unfeasiblePartitionsConnected();
    // Gerar uma lista com os IDs das partições que podem ser fundidas
    unfeasibleConnection whichPartitionToFuseWith(Partition*);
    
    /* 
        VARIÁVEIS UTILIZADAS
    */
    PartitionMap allPartitions;
    PartitionMap unfeasiblePartitions;
    CityMap unitedGraph;

    CityMap red;
    CityMap blue;
    vector<Parent> partitionsChoosen;
    Parent offspringChoosen;

    /* 
        NEW FUNCTIONS
    */
    
    /* bool isPairEqual(const pair<int,int> &p1, const pair<int,int> &p2){
        return((p1.first==p2.first || p1.first==p2.second) && (p1.second==p2.first || p1.second==p2.second));
    } */
};

#endif