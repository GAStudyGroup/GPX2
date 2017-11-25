#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "GPX2.hpp"
#include "ImportData.hpp"
#include "Population.hpp"
#include "Utils.hpp"

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
using std::unordered_map;

void GA(string, unsigned);
bool stop(Population);
Population generateNewPopulation(Population, int);
void addNewTourToPopulation(Population &, vector<int> t);
void cross(unsigned,unsigned,Population &, Population &);

int id{ 0 };
string name{ "" };
mutex muLock;
unsigned number_of_threads;

unordered_map<int,pair<double,double>> map;

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
}

void GA(string name, unsigned popSize)
{

    //Map map;
    Population pop;

    {
        ImportData dataFile(name,map);
        //carrega o mapa
        //map.setCityList(dataFile.getCitiesCoord());

        //carrega a primeira população
        pop = dataFile.importFirstPopulation(map, name, popSize);
    }

    int i{ 1 }, firstBestFitness{ pop.bestFitness(map) };
    cout << "First fitness " << firstBestFitness << endl;
    while (stop(pop)) {

        pop = generateNewPopulation(pop, i);

        /* if (i % 10 == 0) {
            cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        } */
        cout << "gen " << i << " best fitness " << pop.bestFitness(map) << endl;
        i++;
    }
    cout << "THE END" << endl;
    cout << "first best fitness: " << firstBestFitness << endl;
    cout << "gen " << i << " best fitness " << pop.bestFitness(map) << endl;
    cout << "=========================" << endl;
}

bool stop(Population pop)
{

    int static generationsWithoutChange{ 0 };
    int static bestFitness{ pop.bestFitness(map) };
    int currentFitness{ pop.bestFitness(map) };

    unsigned totalCon{ 0 };
    for (vector<int> t : pop.getPopulation()) {
        if (getFitness(t,map) == bestFitness) {
            totalCon++;
        }
    }

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
    vector<int> t;
    for(unsigned i=begin;i<end;i++){
        if (i == (end - 1)) {
            t = (GPX2::crossover(oldPop.getPopulation().at(i), oldPop.getPopulation().at(begin),map));
        } else {
            t = (GPX2::crossover(oldPop.getPopulation().at(i), oldPop.getPopulation().at(i + 1),map));
        }
        addNewTourToPopulation(newPop,t);
    }
}


void addNewTourToPopulation(Population &newPop, vector<int> t){
    lock_guard<mutex> lock(muLock);
    newPop.getPopulation().push_back(t);
}