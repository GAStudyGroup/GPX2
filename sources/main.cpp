#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "Config.hpp"
#include "GPX2.hpp"
#include "ImportData.hpp"
#include "Map.hpp"
#include "Opt.hpp"
#include "Population.hpp"
#include "Utils.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::random_shuffle;
using std::ref;
using std::sort;
using std::srand;
using std::stof;
using std::stoi;
using std::ofstream;

// inicia o algoritmo genético
void GA();

// avalia a população para decidir se as condições de parada foram satisfeitas
bool stop(Population);

// Gera uma nova população
Population generateNewPopulation(Map, Population);

// Gera um tour utilizando Nearest Neighbor
vector<City> nearestNeighbor(Map &);

void fillPopulation(Map &, Population &, unsigned);
Population crossNBestxAllwithReset(Map, Population);
Population crossAllxAllwith2opt(Population);
Population crossAllxAllwithNBestAndReset(Map, Population);

ofstream logFile;

//1 argumento tour_name, 
//2 caminho para o .tsp, 
//3 tamanho da pop, 4 ID da run(usado para log
//5 porcentagem de população inicial gerado pelo LK
//6 tipo de geração da nova poop
// ex: GA berlin52 lib/ 100 0.1 0
int main(int argc, char *argv[]) {

    srand(time(NULL)); 

    if (argc == 7) {
        try {
            Config::NAME = argv[1];
            Config::LIB_PATH = argv[2];
            Config::POP_SIZE = stoi(argv[3]);
            Config::ID = stoi(argv[4]);
            Config::LK_PERCENTAGE = stof(argv[5]);
            Config::NEW_POP_TYPE = stoi(argv[6]);
        } catch (invalid_argument &i_a) {
            cout << "Invalid argument!" << i_a.what() << endl;
            return (0);
        }
    } else {
        cout << "Invalid argument number."
             << endl;
        return (0);
    }

    GA();

    return 0;
}

void fillPopulation(Map &map, Population &pop, unsigned popToFill) {
    for (unsigned i = 0; i < popToFill; i++) {
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
            pair<double, double> p1(cityList[i].second.getX(),
                                    cityList[i].second.getY()),
            p2(tour.back().getX(), tour.back().getY());

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

void GA() {
    Map map;
    Population pop;

    string logName{"Logs/"+to_string(Config::NEW_POP_TYPE)+"/log_"+Config::NAME+"_"+to_string(Config::POP_SIZE)+(Config::LK_PERCENTAGE>0?("_LK"):(""))+".log"};

    logFile.open(logName);
    if(!logFile.is_open()){
        cout<<"falha na abertura do arquivo"<<endl;
        exit(0);
    }

    {
        ImportData dataFile(Config::LIB_PATH+Config::NAME);
        // carrega o mapa
        map.setCityList(dataFile.getCitiesCoord());
        unsigned lkPop = Config::POP_SIZE * Config::LK_PERCENTAGE;
        unsigned fillPop = Config::POP_SIZE - lkPop;
        // carrega a primeira população
        if (Config::LK_PERCENTAGE > 0) {
            pop = dataFile.importFirstPopulation(map, Config::NAME, lkPop);
        }
        fillPopulation(map, pop, fillPop);
    }

    int i{1}, firstBestFitness{pop.bestFitness()};
    logFile << "First fitness " << firstBestFitness << endl;
    do {
        pop = generateNewPopulation(map, pop);

        logFile << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        i++;
    } while (stop(pop));
    logFile << "THE END" << endl;
    logFile << "first best fitness: " << firstBestFitness << endl;
    logFile << "gen " << i << " best fitness " << pop.bestFitness() << endl;
    logFile << "=========================" << endl;

    logFile << "\nLast population" <<endl;
    sort(pop.getPopulation().begin(), pop.getPopulation().end(),
         [](Tour &a, Tour &b) { return a.getFitness() < b.getFitness(); });
    for(Tour t : pop.getPopulation()){
        logFile<<t<<endl;
    }
    logFile.close();
}

bool stop(Population pop) {
    unsigned static generationsWithoutChange{0};
    int static bestFitness{pop.bestFitness()};
    int currentFitness{pop.bestFitness()};

    unsigned totalEqual{1};
    // conta quantas rotas estão com a fitness igual a da melhor rota, para
    // determinar estagnação
    for (Tour t : pop.getPopulation()) {
        if (t.getFitness() == bestFitness) {
            totalEqual++;
        }
    }
    logFile << "total equal: " << totalEqual << ", pop size "<< Config::POP_SIZE << endl;
    // zera o contador de gerações sem mudança se for encontrado uma fitness
    // melhor
    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
    } else if (totalEqual == Config::POP_SIZE) {
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
    if(Config::NEW_POP_TYPE==0){
        return crossAllxAllwith2opt(pop);
    }else if(Config::NEW_POP_TYPE==1){
        return crossNBestxAllwithReset(map, pop);
    }else{
        return crossAllxAllwithNBestAndReset(map, pop);
    }
}

Population crossAllxAllwith2opt(Population pop) {
    Population newPop;
    Tour currentTour;

    for (unsigned i = 0; i < Config::POP_SIZE; i++) {
        currentTour = pop.getPopulation()[i];
        for (unsigned j = 0; j < Config::POP_SIZE; j++) {
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

Population crossNBestxAllwithReset(Map map, Population pop) {
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
    fillPopulation(map, newPop, Config::POP_SIZE - Config::N_BEST);
    return (newPop);
}

Population crossAllxAllwithNBestAndReset(Map map, Population pop){
    Population tmpPop, newPop;
    Tour currentTour;

    for (unsigned i = 0; i < Config::POP_SIZE; i++) {
        currentTour = pop.getPopulation()[i];
        for (unsigned j = 0; j < Config::POP_SIZE; j++) {
            if (i != j) {
                Tour t = GPX2::crossover(pop.getPopulation()[j], currentTour);
                if (t.getFitness() < currentTour.getFitness()) {
                    currentTour = t;
                }
            }
        }
        currentTour = Opt::optimize(currentTour);

        tmpPop.getPopulation().push_back(currentTour);
    }

    sort(tmpPop.getPopulation().begin(), tmpPop.getPopulation().end(),
         [](Tour &a, Tour &b) { return a.getFitness() < b.getFitness(); });

    for(unsigned i=0;i<Config::N_BEST;i++){
        newPop.getPopulation().push_back(tmpPop.getPopulation()[i]);
    }

    fillPopulation(map, newPop, Config::POP_SIZE - Config::N_BEST);
    return newPop;
}