# GPX (Generalized Partition Crossover) for solving the TSP (Travelling Salesperson Problem)
In this repository there are two branches, the *master* branch contains the implementation of a genetic algorithm using GPX, while the *basedev* branch contains the implementation of the GPX and its basic structures (City e Tour) to be used by others.

# Execution steps of GPX

## STEP 1 - Tour Mapping

The Tour structure, initially , it is a city list in which its order represents the city visitation order. 

To be possible of using GPX, it is necessary that the Tour be represented in the form of a graph, in which the vertices are the cities, while the edges are the connections between the cities.

## STEP 2 - Applicating GhostNodes

The GPX uses the GhostNode concept, when two parent graphs are united may exist nodes of degree 4 , or in another words, nodes connected to 4 other vertices.

These vertices can be duplicated, therefore, a ghost node is created in the same spot of the "real" vertex, by doing this is possible to increase the partition number.

## STEP 3 - Union of the Parent Graphs

To be possible to create partitions it's needed that a Graph containing the union of the parents be generated, when the graphs are united, the resulting graph is an overlapping of the two parents. This graph will be called GU.

## STEP 4 - Remove the overlapping edges

After creating the GU it's necessary to remove the overlapping edges, that is, the edges which are in both parents. By doing theses "cuts" the partitions are created.

The cuts will be described as cost 0 edges and also they will be marked as AcessNodes.

## STEP 5 - Finding the Partitions

When executing a GPX, after the creation of the GU' is needed to find "SubTours" that represent partitions, these partitions are used to create the children afterwards.

The partitions are circuits with connections between the the vertices. The higher the partition number the better is the performance of the Crossover.

## STEP 6 - Verify if the partitions are recombinants (feasible)

To be possible of executing the GPX it's also needed that the partitions be recombinant, that is, that they have the same AcessNodes that their parents.

This means that is possible to go throught the same subtour in both parents.

If a partition does not follow this rule, so it is a non-recombinant partition, which we call unfeasible. In order to improve performance, the GPX2 try to fuse unfeasible partitions, so they might become feasible, hence, increasing the partition number.

## STEP 7 - Choose the "SubTours" that will compound the child

In this step it will be veryfied between the parents which of the two have the better SubTour to be inherited by the child. Each partition is made up by the union of a SubTour from each parent, the best SubTour is choose and saved. 

## STEP 9 - Converting the graph to a Cities List (Tour) 

When the GPX is finished, the GPX will transform the child graph into a tour again.

# ----------------------------------------------------------------

# Crossover GPX implementado para o problema do TSP 

Implementação do Generalized Partition Crossover para o problema do Traveling Salesman Problem.

Existem dois Branchs neste repositório, o Branch *master* contém a implementação de um algoritmo genético utilizando o GPX e o Branch *basedev* contém a implementação do GPX e as estruturas básicas que precisam ser utilizadas, City e Tour, para fins de uso por terceiros.

# Etapas da execução do GPX

## STEP 1 - Mapeamento do Tour

A estrutura Tour, inicialmente, é uma lista de cidades no qual a ordem que elas estão dispostas representa a ordem de visitação.

Para a realização do GPX é necessário que o Tour esteja disposto na forma de um grafo, com vértices sendo as cidades e arestas as ligações entre elas.
 
## STEP 2 - Adição dos GhostNodes

O GPX implementa a ideia de GhostNode, quando os dois grafos pais são unidos podem existir nós que possuam grau 4 (ligações com outros 4 vértices). 

Esses vertices podem ser duplicados, gerando um vértice fantasma no mesmo ponto do vértice real, com isso é possível aumentar o total de partições encontradas.

Teóricamente os GhostNodes são adicionados ao grafo após a união dos grafos dos pais, porém para cortar custos computacionais eles são executados antes da união, sendo assim quando os grafos são unidos os GhostNodes já são inseridos no grafo da união.

## STEP 3 - União dos Grafos dos pais

Para a criação das partições é necessário que um Grafo com a união dos pais seja gerado (GU), como se os dois pais fossem sobrepostos.

## STEP 4 - Cortar as arestas sobrepostas

Após a criação do GU é necessário retirar as arestas sobrepostas, ou seja, as arestas que estão nos dois pais. Com esse corte as partições "começam a aparecer".
        
Os cortes são simbolizados com arestas de custo 0 de ligação e são marcados como AccessNodes.

## STEP 5 - Encontrar as partições
        
Para a execução do GPX, após a criação do GU' é preciso encontrar "SubTours" que simbolizem partições, essas partições serão utilizadas na formação do filho posteriormente.
        
As partições são circuitos com ligações entre os vértices desses cricuitos. Quanto maior o total de partições melhor será o desempenho do Crossover.

## STEP 6 - Checar se as partições são recombinantes (feasible)

Para poder realizar o Crossover é necessário que as partições que serão utilizadas para a criação do filho sejam recombinantes, ou seja, possuam AccessNodes de entrada e saída da partição representados em seus pais. 
        
No caso, seria possível percorrer uma entrada e uma saída tanto em um pai quanto no outro.

Se uma partição não seguir essa regra, ela se torna não-recombinantes (unfeasible). O GPX implementa uma "solução" para isso, a fim de melhorar o desempenho ele tenta fundir as partições unfeasible para torná-las feasible, caso possível.
        
Se ocorrer de uma fusão gerar uma partição feasible, ela será colocada na lista de partições feasible.

## STEP 7 - Escolher os "SubTours" que irão compor o filho

Nesta etapa será verificado em cada pai qual possui o melhor SubTour para ser herdado pelo filho. Cada partição é composta pela união de um SubTour de cada pai, eles serão escolhidos e salvos de acordo com o melhor.

O método irá executar a verificação em cada partição, salvando em uma lista o pai com o melhor SubTour.

## STEP 8 - Criar Grafo do filho

Após escolhidos os melhores SubTours, o mapa do filho será criado.

Depois de criado é necessário remover os GhostNodes, eles serão unidos aos vértices de origem.

Para melhoria, após a montagem do filho sobre os pais, é verificado qual Tour, Red ou Blue, ficou com uma fitness mais elevada, esse Tour será escolhido para ser o filho.

## STEP 9 - Linearização de Grafo para lista de City (Tour)

Após terminado, o GPX irá retornar o Grafo filho ao estado original dos pais, forma de um Tour.
