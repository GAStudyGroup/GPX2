#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#include "GPX2.hpp"
#include "ImportData.hpp"
#include "Map.hpp"
#include "Population.hpp"
#include "Opt.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::random_shuffle;
using std::srand;
using std::stoi;
using std::mutex;
using std::lock_guard;
using std::thread;
using std::ref;

void GA(string, unsigned);
bool stop(Population);
Population generateNewPopulation(Population, int);
void addNewTourToPopulation(Population &, Tour t);
void cross(unsigned,unsigned,Population &, Population &);

int id{ 0 };
string name{ "" };
mutex muLock;
unsigned number_of_threads;


//primeiro argumento tour_name, segundo tamanho da pop
int main(int argc, char* argv[])
{
    
    unsigned popSize{ 0 };

    srand(time(NULL));

    if (argc == 4) {
        try {
            name = argv[1];
            popSize = stoi(argv[2]);
            id = stoi(argv[3]);
            number_of_threads = thread::hardware_concurrency();
        } catch (invalid_argument& i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 5) {
        try {
            name = argv[1];
            popSize = stoi(argv[2]);
            id = stoi(argv[3]);
            number_of_threads = stoi(argv[4]);
        } catch (invalid_argument& i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 0) {
        cout << "Please specify the arguments: tour name and population size." << endl;
        return (0);
    } else {
        cout << "Invalid argument number. Expecting two arguments." << endl;
        return (0);
    }

    GA(name, popSize);

    return 0;

   

    /* srand(time(NULL));

    vector<City> vec,vec2;
    vec.push_back(City(0,0,0));
    vec.push_back(City(1,10,0));
    vec.push_back(City(2,10,10));
    vec.push_back(City(3,0,20));
    vec.push_back(City(4,0,30));
    vec.push_back(City(5,10,30));
    vec.push_back(City(6,10,20));
    vec.push_back(City(7,0,10));


    Tour t(vec);


    cout<<"fitness before "<<t.getFitness()<<endl;
    t = Opt::optimize(t);
    cout<<"fitness after "<<t.getFitness()<<endl;
    return 0; */
} 

void fillPopulation(Map &map, Population &pop, int popSize){
    for(int i=0;i<(popSize*0.9);i++){
        vector<City> newTour{map.getCityList()};
        std::random_shuffle(newTour.begin(),newTour.end());
        Tour t(newTour);
        t = Opt::optimize(t);
        pop.getPopulation().push_back(t);
    }
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
        pop = dataFile.importFirstPopulation(map, name, popSize*0.1);
        fillPopulation(map,pop,popSize);

        cout<<"gerou pop"<<endl;
        cout<<"pop size "<<pop.getPopulation().size()<<endl;
    }

    int i{ 1 }, firstBestFitness{ pop.bestFitness() };
    cout << "First fitness " << firstBestFitness << endl;
    while (stop(pop)) {

        pop = generateNewPopulation(pop, i);

        /* if (i % 10 == 0) {
            cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        } */
        cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        i++;
    }
    cout << "THE END" << endl;
    cout << "first best fitness: " << firstBestFitness << endl;
    cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
    cout << "=========================" << endl;
}

bool stop(Population pop)
{

    int static generationsWithoutChange{ 0 };
    int static bestFitness{ pop.bestFitness() };
    int currentFitness{ pop.bestFitness() };

    unsigned totalCon{ 0 };
    for (Tour t : pop.getPopulation()) {
        if (t.getFitness() == bestFitness) {
            totalCon++;
        }
    }
    cout<<"totalCon: "<<totalCon<<endl;
    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
    } else if (totalCon == pop.getPopulation().size()) {
        return (false);
    } else {
        generationsWithoutChange++;
    }
    if (generationsWithoutChange == 100) {
        return (false);
    } else {
        return (true);
    }
}

Population generateNewPopulation(Population pop, int gen)
{
    unsigned size = pop.getPopulation().size();
    Population newPop;

    std::random_shuffle(pop.getPopulation().begin(), pop.getPopulation().end());
    vector<thread> threads;
    threads.reserve(number_of_threads);
    unsigned entrysPerThread = size/number_of_threads;

    for(unsigned i=0;i<number_of_threads;i++){
        if(i==(number_of_threads-1)){
            threads.emplace_back(thread(cross,(entrysPerThread*i),(size),ref(pop),ref(newPop)));
        }else{
            threads.emplace_back(thread(cross,(entrysPerThread*i),(entrysPerThread*(i+1)),ref(pop),ref(newPop)));
        }
    }
    for(thread &t : threads){
        t.join();
    }

    return newPop;
}

void cross(unsigned begin,unsigned end,Population &oldPop, Population &newPop){
    Tour t;
    for(unsigned i=begin;i<end;i++){
        if (i == (end - 1)) {
            t = (GPX2::crossover(oldPop.getPopulation().at(i), oldPop.getPopulation().at(begin)));
        } else {
            t = (GPX2::crossover(oldPop.getPopulation().at(i), oldPop.getPopulation().at(i + 1)));
        }
        addNewTourToPopulation(newPop,t);
    }
}


void addNewTourToPopulation(Population &newPop, Tour t){
    lock_guard<mutex> lock(muLock);
    newPop.addNewTour(t);
}