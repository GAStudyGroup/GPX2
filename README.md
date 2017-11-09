# Crossover GPX implementado para o problema do TSP

Implementação do Generalized Partition Crossover para o problema do Traveling Salesman Problem.

Existem dois Branchs neste repositório, o Branch *master* contém a implementação de um algoritmo genético utilizando o GPX e o Branch *basedev* contém a implementação do GPX e as estruturas básicas que precisam ser utilizadas, City e Tour, para fins de uso por terceiros.


# AG TODO

- [ ] Executar LKH para gerar a pop Inicial
- [ ] Ler os arquivos e carregar como popInicial
- [ ] Cruzar par a par para gerar nova Pop
- [ ] Gerar log do melhor indíviduo por geração
- [ ] Critério de parada por estagnação

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
