#include <algorithm> 
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "GPX2.hpp"
#include "ImportData.hpp"
#include "Map.hpp"
#include "Opt.hpp"
#include "Population.hpp"
#include "Utils.hpp"
#include "Config.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::random_shuffle;
using std::ref;
using std::sort;
using std::srand;
using std::stoi;

// inicia o algoritmo genético
void GA(unsigned);

// avalia a população para decidir se as condições de parada foram satisfeitas
bool stop(Population);

// Gera uma nova população
Population generateNewPopulation(Map, Population);

// Gera um tour utilizando Nearest Neighbor
vector<City> nearestNeighbor(Map &);

void fillPopulation(Map &, Population &, int);

Population crossAllxAllwithReset(Map, Population);

Population crossAllxAllwith2opt(Population);

int ID{0};
string NAME{""};
Config::type TYPE = Config::type::EUC_2D;

// primeiro argumento tour_name, segundo tamanho da pop, terceiro ID da
// run(usado para log)  ex: GA berlin52 100 0
int main(int argc, char *argv[]) {
    unsigned popSize{0};

    srand(time(NULL));

    if (argc == 4) {
        try {
            NAME = argv[1];
            popSize = stoi(argv[2]);
            ID = stoi(argv[3]);
        } catch (invalid_argument &i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 5) {
        try {
            NAME = argv[1];
            popSize = stoi(argv[2]);
            ID = stoi(argv[3]);
        } catch (invalid_argument &i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 0) {
        cout << "Please specify the arguments: tour name and population size."
             << endl;
        return (0);
    } else {
        cout << "Invalid argument number. Expecting two or tree arguments."
             << endl;
        return (0);
    }

    GA(popSize);

    return 0;
}

void fillPopulation(Map &map, Population &pop, int popToFill) {
    for (int i = 0; i < popToFill; i++) {
        // Tour t(nearestNeighbor(map));
        Tour t(map.getCityList());
        random_shuffle(t.getRoute().begin(), t.getRoute().end());
        t = Opt::optimize(t);
        pop.getPopulation().push_back(t);
    }
}

vector<City> nearestNeighbor(Map &map) {
    vector<pair<float, City>> cityList;
    for (City c : map.getCityList()) {
        cityList.push_back(make_pair(0, c));
    }
    unsigned choosenCity = rand() % cityList.size();
    vector<City> tour;
    tour.push_back(cityList[choosenCity].second);
    cityList.erase(cityList.begin() + choosenCity);
    while (!cityList.empty()) {
        for (unsigned i = 0; i < cityList.size(); i++) {
            pair<double,double> p1(cityList[i].second.getX(),cityList[i].second.getY()),p2(tour.back().getX(),tour.back().getY());

            cityList[i].first = distance(p1, p2);
        }
        sort(cityList.begin(), cityList.end(), [](auto &left, auto &right) {
            return (right.first > left.first);
        });
        tour.push_back(cityList[0].second);
        cityList.erase(cityList.begin());
    }
    return (tour);
}

void GA(unsigned popSize) {
    Map map;
    Population pop;

    {
        ImportData dataFile(NAME);
        // carrega o mapa
        map.setCityList(dataFile.getCitiesCoord());
        cout << "importou map" << endl;

        Tour t(map.getCityList());

        /* // carrega a primeira população
        // pop = dataFile.importFirstPopulation(map, name, popSize*LKpop);
        cout << "filling population" << endl;
        fillPopulation(map, pop, popSize * (1 - Config::LK_PERCENTAGE));

        cout << "gerou pop" << endl;
        cout << "pop size " << pop.getPopulation().size() << endl; */
    }

    /* int i{1}, firstBestFitness{pop.bestFitness()};
    cout << "First fitness " << firstBestFitness << endl;
    do {
        pop = generateNewPopulation(map, pop);

        cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        i++;
    } while (stop(pop));
    cout << "THE END" << endl;
    cout << "first best fitness: " << firstBestFitness << endl;
    cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
    cout << "=========================" << endl; */
}

bool stop(Population pop) {
    unsigned static generationsWithoutChange{0};
    int static bestFitness{pop.bestFitness()};
    int currentFitness{pop.bestFitness()};

    unsigned totalCon{0};
    // conta quantas rotas estão com a fitness igual a da melhor rota, para
    // determinar estagnação
    for (Tour t : pop.getPopulation()) {
        if (t.getFitness() == bestFitness) {
            totalCon++;
        }
    }
    cout << "totalCon: " << totalCon << ", pop size "
         << pop.getPopulation().size() << endl;
    // zera o contador de gerações sem mudança se for encontrado uma fitness
    // melhor
    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
    } else if (totalCon == pop.getPopulation().size()) {
        // se todos os elementos foram iguais termina o GA
        return (false);
    } else {
        generationsWithoutChange++;
    }
    // limite de gerações
    if (generationsWithoutChange >= Config::GENERATION_LIMIT) {
        return (false);
    } else {
        return (true);
    }
}

Population generateNewPopulation(Map map, Population pop) {
    return crossAllxAllwithReset(map, pop);
}

Population crossAllxAllwith2opt(Population pop) {
    unsigned size = pop.getPopulation().size();
    Population newPop;
    Tour currentTour;

    for (unsigned i = 0; i < size; i++) {
        currentTour = pop.getPopulation()[i];
        for (unsigned j = 0; j < size; j++) {
            if (i != j) {
                Tour t = GPX2::crossover(pop.getPopulation()[j], currentTour);
                if (t.getFitness() < currentTour.getFitness()) {
                    currentTour = t;
                }
            }
        }
        currentTour = Opt::optimize(currentTour);

        newPop.getPopulation().push_back(currentTour);
    }

    return newPop;
}

Population crossAllxAllwithReset(Map map, Population pop) {
    Population newPop;
    // dar sort na pop
    sort(pop.getPopulation().begin(), pop.getPopulation().end(),
         [](Tour &a, Tour &b) { return a.getFitness() < b.getFitness(); });

    // cruzar os 3 primeiros com todos
    for (unsigned i = 0; i < Config::N_BEST; i++) {
        Tour savedTour = pop.getPopulation()[i];
        for (Tour t : pop.getPopulation()) {
            Tour tmp = GPX2::crossover(savedTour, t);
            savedTour = tmp;
        }
        newPop.getPopulation().push_back(savedTour);
    }

    // preencher o resto da pop com novos tours gerados com NN e 2opt
    fillPopulation(map, newPop, pop.getPopulation().size() - Config::N_BEST);
    return (newPop);
}