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
Population generateNewPopulation(Population, int);
void addNewTourToPopulation(Population &, Tour t);
void cross(unsigned,unsigned,Population &, Population &);
vector<City> nearestNeighbor(Map &);
double distance(const City &, const City &);
void swap(Tour&, const int, const int);
int findElement(Tour&, const int);
Tour crossover(Tour& parent1, Tour& parent2);

double LKpop{0.2};
int id{ 0 };
string name{ "" };
/* mutex muLock;
unsigned number_of_threads; */


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
        pop = dataFile.importFirstPopulation(map, name, popSize*LKpop);
        cout<<"filling population"<<endl;
        fillPopulation(map,pop,popSize*(1-LKpop));

        cout<<"gerou pop"<<endl;
        cout<<"pop size "<<pop.getPopulation().size()<<endl;
    }

    int i{ 1 }, firstBestFitness{ pop.bestFitness() };
    cout << "First fitness " << firstBestFitness << endl;
    do{

        pop = generateNewPopulation(pop, i);

        /* if (i % 10 == 0) {
            cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        } */
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

Population generateNewPopulation(Population pop, int gen)
{
    unsigned size = pop.getPopulation().size();
    Population newPop;
    //std::random_shuffle(pop.getPopulation().begin(), pop.getPopulation().end());

    vector<pair<int,int>> tourList;
    for(unsigned i=0;i<size;i++){
        tourList.push_back(make_pair(i,pop.getPopulation()[i].getFitness()));
    }

    sort(tourList.begin(),tourList.end(),[](auto &left,auto &right){
            return(right.second>left.second);});

    for(unsigned i=0;i<3;i++){
        Tour bestTour{pop.getPopulation()[tourList[i].first]}, savedTour{bestTour};
        cout<<"begining work in "<<(i+1)<<" best "<<savedTour.getFitness()<<endl;
        for(unsigned j=0;j<size;j++){
            if(i!=j){
                Tour t = GPX2::crossover(pop.getPopulation()[j],bestTour);
                if(t.getFitness()<bestTour.getFitness()){
                    cout<<"crossed GPX2: "<<bestTour.getFitness()<<" with "<<pop.getPopulation()[j].getFitness()<<endl;
                    cout<<"new best tour "<<t.getFitness()<<endl;
                    bestTour = t;
                }
            }
        }


        cout<<"inserting in new Pop: "<<endl;

        cout<<"saved tour "<<savedTour.getFitness()<<endl;
        cout<<"best tour "<<bestTour.getFitness()<<endl;

        newPop.getPopulation().push_back(savedTour);
        newPop.getPopulation().push_back(bestTour);

    }

    {
        unsigned choosen;
        long unsigned alreadyChoosen{newPop.getPopulation().size()};
        for(unsigned i=0;i<(size-alreadyChoosen);i++){
            choosen = rand()%size;
            if(rand()%100 < 30){
                unsigned crossWith = rand()%size;
                cout<<"=========================//========================\nvai fazer cross:\n\t1: "<<pop.getPopulation()[choosen].getFitness()<<"\n\t2: "<<pop.getPopulation()[crossWith].getFitness()<<endl;
                Tour tmp{crossover(pop.getPopulation()[choosen],pop.getPopulation()[crossWith])};
                newPop.getPopulation().push_back(Opt::optimize(tmp));
                cout<<"offspring "<<newPop.getPopulation().back().getFitness()<<endl;
            }else{
                newPop.getPopulation().push_back(Opt::optimize(pop.getPopulation()[choosen]));
            }
        }
        /* for(unsigned i=0;i<(size-alreadyChoosen);i++){
            choosen = rand()%size;
            newPop.getPopulation().push_back(pop.getPopulation()[choosen]);
        } */
    }

    /* for(unsigned i=0;i<size;i++){
        newPop.getPopulation().push_back(GPX2::crossover(pop.getPopulation()[i],pop.getPopulation()[(i+1)%size]));
        cout<<"crossover "<<i<<" over"<<endl;
    }
 */
    /* std::random_shuffle(pop.getPopulation().begin(), pop.getPopulation().end());
    vector<thread> threads;
    threads.reserve(number_of_threads);
    unsigned entrysPerThread = size/number_of_threads; */

    /* for(unsigned i=0;i<number_of_threads;i++){
        if(i==(number_of_threads-1)){
            threads.emplace_back(thread(cross,(entrysPerThread*i),(size),ref(pop),ref(newPop)));
        }else{
            threads.emplace_back(thread(cross,(entrysPerThread*i),(entrysPerThread*(i+1)),ref(pop),ref(newPop)));
        }
    }
    for(thread &t : threads){
        t.join();
    } */

    return newPop;
}

/* void cross(unsigned begin,unsigned end,Population &oldPop, Population &newPop){
    Tour t;
    for(unsigned i=begin;i<end;i++){
        if (i == (end - 1)) {
            t = (GPX2::crossover(oldPop.getPopulation().at(i), oldPop.getPopulation().at(begin)));
        } else {
            t = (GPX2::crossover(oldPop.getPopulation().at(i), oldPop.getPopulation().at(i + 1)));
        }
        addNewTourToPopulation(newPop,t);
    }
} */


/* void addNewTourToPopulation(Population &newPop, Tour t){
    lock_guard<mutex> lock(muLock);
    newPop.addNewTour(t);
} */

double distance(const City &a, const City &b)
{ // Retorno da distancia entre duas cidades
    return (std::round(sqrt(pow(a.getX()-b.getX(),2) + pow((a.getY()) - (b.getY()), 2)))); 
}


void swap(Tour& vector, const int a, const int b){
    City tmp = vector.getRoute()[a];
    vector.getRoute()[a]=vector.getRoute()[b];
    vector.getRoute()[b]=tmp;
}

int findElement(Tour& vector, const int element){
    for(unsigned i=0; i<vector.getRoute().size(); i++){
        if(vector.getRoute()[i].getId() == element) return (i);
    }
    return(-1);
}

Tour crossover(Tour& parent1, Tour& parent2){
    Tour tmp1{parent1}, tmp2{parent2};
    long unsigned cutPoint{parent1.getRoute().size()/2},infLimit{parent1.getRoute().size()};

        cutPoint = ((parent1.getRoute().size() + parent2.getRoute().size())/4);
        if(parent1.getRoute().size() > parent2.getRoute().size()){
            infLimit = parent2.getRoute().size();
        }
        if(infLimit < cutPoint){
            cutPoint = infLimit;
        }
    

    for(unsigned i=cutPoint; i<infLimit; i++){
        swap(tmp1, i, findElement(tmp1, parent2.getRoute()[i].getId()));
        tmp1.getRoute()[i] = parent2.getRoute()[i];
        swap(tmp2, i, findElement(tmp2, parent1.getRoute()[i].getId()));
        tmp2.getRoute()[i] = parent1.getRoute()[i];
    }
    return((tmp1.getFitness()<tmp2.getFitness())?(tmp1):(tmp2));   
}