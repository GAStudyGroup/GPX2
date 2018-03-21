#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <cmath>

#include "GPX2.hpp"
#include "ImportData.hpp"
#include "Map.hpp"
#include "Population.hpp"
#include "Opt.hpp"
#include "Utils.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::random_shuffle;
using std::srand;
using std::stoi;
using std::ref;
using std::sort;

//inicia o algoritmo genético
void GA(string, unsigned);

//avalia a população para decidir se as condições de parada foram satisfeitas
bool stop(Population);

//Gera uma nova população
Population generateNewPopulation(Population, int);

//Gera um tour utilizando Nearest Neighbor
vector<City> nearestNeighbor(Map &);

// int distance(const City &, const City &);
unsigned GENERATION_LIMIT{10};
double LKpop{0};
int id{ 0 };
string name{""};


//primeiro argumento tour_name, segundo tamanho da pop, terceiro ID da run(usado para log)
//ex: GA berlin52 100 0
int main(int argc, char* argv[])
{
    unsigned popSize{ 0 };

    srand(time(NULL));

    if (argc == 4) {
        try {
            name = argv[1];
            popSize = stoi(argv[2]);
            id = stoi(argv[3]);
        } catch (invalid_argument& i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 5) {
        try {
            name = argv[1];
            popSize = stoi(argv[2]);
            id = stoi(argv[3]);
        } catch (invalid_argument& i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 0) {
        cout << "Please specify the arguments: tour name and population size." << endl;
        return (0);
    } else {
        cout << "Invalid argument number. Expecting two or tree arguments." << endl;
        return (0);
    }

    GA(name, popSize);

    return 0;
} 

void fillPopulation(Map &map, Population &pop, int popSize){
    for(int i=0;i<popSize;i++){
        Tour t(nearestNeighbor(map));
        t = Opt::optimize(t);
        pop.getPopulation().push_back(t);
    }
}

vector<City> nearestNeighbor(Map &map){
    vector<pair<float,City>> cityList;
    for(City c : map.getCityList()){
        cityList.push_back(make_pair(0,c));
    }
    unsigned choosenCity = rand() % cityList.size(); 
    vector<City> tour;
    tour.push_back(cityList[choosenCity].second);
    cityList.erase(cityList.begin()+choosenCity);
    while(!cityList.empty()){
        for(unsigned i=0;i<cityList.size();i++){
            cityList[i].first = distance(cityList[i].second,tour.back());
        }
        sort(cityList.begin(),cityList.end(),[](auto &left,auto &right){
            return(right.first>left.first);
        });
        tour.push_back(cityList[0].second);
        cityList.erase(cityList.begin());
    }
    return(tour);
}

void GA(string name, unsigned popSize)
{

    Map map;
    Population pop;

    {
        ImportData dataFile(name);
        //carrega o mapa
        map.setCityList(dataFile.getCitiesCoord());
        cout<<"importou map"<<endl;

        //carrega a primeira população
        // pop = dataFile.importFirstPopulation(map, name, popSize*LKpop);
        cout<<"filling population"<<endl;
        fillPopulation(map,pop,popSize*(1-LKpop));

        cout<<"gerou pop"<<endl;
        cout<<"pop size "<<pop.getPopulation().size()<<endl;
    }

    int i{ 1 }, firstBestFitness{ pop.bestFitness() };
    cout << "First fitness " << firstBestFitness << endl;
    do{

        pop = generateNewPopulation(pop, i);

        cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        i++;
    }while (stop(pop));
    cout << "THE END" << endl;
    cout << "first best fitness: " << firstBestFitness << endl;
    cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
    cout << "=========================" << endl;
}

bool stop(Population pop)
{
    unsigned static generationsWithoutChange{ 0 };
    int static bestFitness{ pop.bestFitness() };
    int currentFitness{ pop.bestFitness() };

    unsigned totalCon{ 0 };
    //conta quantas rotas estão com a fitness igual a da melhor rota, para determinar estagnação
    for (Tour t : pop.getPopulation()) {
        if (t.getFitness() == bestFitness) {
            totalCon++;
        }
    }
    cout<<"totalCon: "<<totalCon<<", pop size "<<pop.getPopulation().size()<<endl;
    //zera o contador de gerações sem mudança se for encontrado uma fitness melhor
    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
    } else if (totalCon == pop.getPopulation().size()) {
        //se todos os elementos foram iguais termina o GA
        return (false);
    } else {
        generationsWithoutChange++;
    }
    //limite de gerações
    if (generationsWithoutChange >= GENERATION_LIMIT) {
        return (false);
    } else {
        return (true);
    }
}

Population generateNewPopulation(Population pop, int gen)
{
    unsigned size = pop.getPopulation().size();
    Population newPop;
    Tour currentTour;

    for(unsigned i=0;i<size;i++){
        currentTour = pop.getPopulation()[i];
        for(unsigned j=0;j<size;j++){
            if(i!=j){
                Tour t = GPX2::crossover(pop.getPopulation()[j],currentTour);
                if(t.getFitness()<currentTour.getFitness()){
                    currentTour = t;
                }
            }
        }
        currentTour = Opt::optimize(currentTour);

        newPop.getPopulation().push_back(currentTour);
    }


    return newPop;
}