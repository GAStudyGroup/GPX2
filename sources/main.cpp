#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <cmath>
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
using std::sort;

void GA(string, unsigned);
bool stop(Population);
Population generateNewPopulation(Map&,Population, int);
void addNewTourToPopulation(Population &, Tour t);
void cross(unsigned,unsigned,Population &, Population &);
vector<City> nearestNeighbor(Map &);
double distance(const City &, const City &);
void fillPopulation(Map &, Population &, int);

double LKpop{0.2};
int id{ 0 };
string name{ "" };


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
            /* number_of_threads = thread::hardware_concurrency(); */
        } catch (invalid_argument& i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 5) {
        try {
            name = argv[1];
            popSize = stoi(argv[2]);
            id = stoi(argv[3]);
            /* number_of_threads = stoi(argv[4]); */
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

void fillPopulation(Map &map, Population &pop, int popSize){
    for(int i=0;i<popSize;i++){
        vector<City> tmpMap = map.getCityList();
        random_shuffle(tmpMap.begin(),tmpMap.end());
        Tour t(tmpMap);
        // Tour t(nearestNeighbor(map));
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
        fillPopulation(map,pop,popSize);

        cout<<"gerou pop"<<endl;
        cout<<"pop size "<<pop.getPopulation().size()<<endl;
    }

    int i{ 1 }, firstBestFitness{ pop.bestFitness() };
    cout << "First fitness " << firstBestFitness << endl;
    do{

        pop = generateNewPopulation(map,pop, i);

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
    int static generationsWithoutChange{ 0 };
    int static bestFitness{ pop.bestFitness() };
    int currentFitness{ pop.bestFitness() };

    unsigned totalCon{ 0 };
    for (Tour t : pop.getPopulation()) {
        if (t.getFitness() == bestFitness) {
            totalCon++;
        }
    }
    cout<<"totalCon: "<<totalCon<<", pop size "<<pop.getPopulation().size()<<endl;
    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
    } else if (totalCon == pop.getPopulation().size()) {
        return (false);
    } else {
        generationsWithoutChange++;
    }
    if (generationsWithoutChange == 5) {
        return (false);
    } else {
        return (true);
    }
}

Population generateNewPopulation(Map &map, Population pop, int gen)
{
    unsigned size = pop.getPopulation().size();
    Population newPop;
    Tour currentTour;
    
    /* currentTour = pop.getPopulation()[0];;
    for(unsigned i=1;i<size;i++){
        Tour t = GPX2::crossover(currentTour,pop.getPopulation()[i]);
        currentTour = t;
    }
    ;

    newPop.getPopulation().push_back(currentTour);

    fillPopulation(map,newPop,size-1); */

    for(unsigned i=0;i<size;i++){
        currentTour = pop.getPopulation()[i];
        int currentFitness = currentTour.getFitness();
        // cout<<"begining work in "<<(i+1)<<", fitness: "<<currentFitness<<endl;
        for(unsigned j=0;j<size;j++){
            if(i!=j){
                Tour t = GPX2::crossover(pop.getPopulation()[j],currentTour);
                if(t.getFitness()<currentTour.getFitness()){
                    // cout<<"crossed GPX2: "<<currentTour.getFitness()<<" with "<<pop.getPopulation()[j].getFitness()<<endl;
                    // cout<<"new best tour "<<t.getFitness()<<endl;
                    currentTour = t;
                }
            }
        }


        // cout<<"inserting in new Pop: "<<endl;

        // cout<<"new Tour "<<currentTour.getFitness()<<endl;
        // cout<<"Applying 2-opt..."<<endl;
        currentTour = Opt::optimize(currentTour);
        // cout<<"new fitness: "<<currentTour.getFitness()<<endl;

        // std::cin.get();

        newPop.getPopulation().push_back(currentTour);

    }

    return newPop;
}


double distance(const City &a, const City &b)
{ // Retorno da distancia entre duas cidades
    return (std::round(sqrt(pow(a.getX()-b.getX(),2) + pow((a.getY()) - (b.getY()), 2)))); 
}