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

int AntColony::iteration = 0;

Population AntColony::run(unsigned popToFill){
    vector<Ant> colony;
    uniform_int_distribution<int> dist(1,Globals::TOUR_SIZE);
    unsigned numberOfAnts{popToFill>Globals::TOUR_SIZE?popToFill:Globals::TOUR_SIZE};

    for(unsigned i=0;i<numberOfAnts;i++){
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
    for(unsigned i=0;i<popToFill;i++){
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