#include "AntColony.hpp"

#include "Utils.hpp"

#include "Globals.hpp"

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::to_string;

#include "Constants.hpp"

#include <random>
using std::uniform_int_distribution;

#include <algorithm>
using std::sort;

#include "Opt.hpp"

AntColony::AntColony(){
    //Creating and initializing matrix of pheromones
    AntMap::pheromoneMap = new double*[Globals::TOUR_SIZE];
    for(unsigned i=0;i<Globals::TOUR_SIZE;i++){
        AntMap::pheromoneMap[i] = new double[Globals::TOUR_SIZE];
    }

    //initialized with 1/(numberOfAnts*(distance of i to j))
    for(unsigned i=1;i<=Globals::TOUR_SIZE;i++){
        for(unsigned j=1;j<=Globals::TOUR_SIZE;j++){
            AntMap::setPheromoneLevel(i,j,i!=j?(1.0/(Globals::TOUR_SIZE*distance(i,j))):(0.0));
        }
    }

    AntMap::bestAntYet.setPath(Globals::map.getCityList());
}


AntColony::~AntColony(){
    for(unsigned i=0;i<Globals::TOUR_SIZE;i++){
        delete AntMap::pheromoneMap[i];
    }
    delete AntMap::pheromoneMap;
}


Population AntColony::run(){
    vector<Ant> colony;
    uniform_int_distribution<int> dist(1,Globals::TOUR_SIZE);

    for(unsigned i=0;i<Globals::TOUR_SIZE;i++){
        //create ant
        Ant newAnt(Globals::map.getCityList()[i%Globals::TOUR_SIZE]);
        // Ant newAnt(dist(Globals::urng));
        //create path
        newAnt.run();
        //store in vector
        colony.push_back(newAnt);
    }

    //sort colony by fitness of ant path
    sort(colony.begin(),colony.end(),[](Ant& a, Ant&b){return getFitness(a.getPath())<getFitness(b.getPath());});

    if(getFitness(AntMap::bestAntYet.getPath())>getFitness(colony.front().getPath())){
        AntMap::bestAntYet = colony.front();
        AntMap::bestFitnessYet = getFitness(AntMap::bestAntYet.getPath());
    }

    // cout<<"iterations without improvement: "<<iterationsWithoutImprovement<<endl;
    //update pheromone with best ant of run
    AntMap::updatePheromoneMap(iteration%5==0?colony.front().getPath():AntMap::bestAntYet.getPath());

    // bestFitnessYet = Config::BEST_FITNESS!=0?Config::BEST_FITNESS:(bestFitnessYet>getFitness(colony.front().getPath())?(getFitness(colony.front().getPath())):(bestFitnessYet));

    Population newPop;
    for(unsigned i=0;i<Config::POP_SIZE;i++){
        newPop.getPopulation().push_back(colony[i].getPath());
    }

    iteration++;
    return newPop;
}


void AntColony::printColony(vector<Ant> colony){
    cout<<"colony size: "<<colony.size()<<endl;
    for(Ant ant : colony){
        cout<<getFitness(ant.getPath())<<endl;
    }
}