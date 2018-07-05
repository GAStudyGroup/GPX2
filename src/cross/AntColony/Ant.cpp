#include "Ant.hpp"

#include "Globals.hpp"

#include "Constants.hpp"

#include "Utils.hpp"

#include <algorithm>
using std::find;
using std::sort;

//utility
using std::make_pair;

#include <random>
using std::uniform_real_distribution;

#include <iostream>
using std::cout;
using std::endl;
using std::cin;

//initialize path with initial city and get copy of pheromoneMap
Ant::Ant(int init){
    path.push_back(init);
}

Ant::Ant(){}

double Ant::calcChance(int nextCity){
    double pheromoneNext = AntMap::getPheromoneLevel(path.back(),nextCity);
    double niNext = 1.0/distance(path.back(),nextCity);
    double top = ((pow(pheromoneNext,AntConstants::alfa))*(pow(niNext,AntConstants::beta)));

    double bottom = 0.0;
    {
        double pheromoneTmp = 0.0;
        double niTmp = 0.0;
        for(unsigned city : Globals::map.getCityList()){
            if(find(path.begin(),path.end(),city) == path.end()){
                pheromoneTmp = AntMap::getPheromoneLevel(path.back(),city);
                niTmp = 1.0/distance(path.back(),city);
                bottom += ((pow(pheromoneTmp,AntConstants::alfa))*(pow(niTmp,AntConstants::beta)));
            }
        }   
    }
    return top/bottom;
}

void Ant::run(){
    while(path.size()<Globals::TOUR_SIZE){
        path.push_back(nextStep());
    }
}

int Ant::nextStep(){
    vector<Choice> choices;
    double sum{0.0},rng{0.0};

    for(unsigned city : Globals::map.getCityList()){
        if(find(path.begin(),path.end(),city) == path.end()){
            choices.push_back(make_pair(city,calcChance(city)));   
        }
    }

    sort(choices.begin(),choices.end(),[](Choice& a, Choice& b){return a.second<b.second;});
    
    for(Choice c : choices){
        sum+=c.second;
    }
    
    uniform_real_distribution<double> dist(0.0,sum);
    
    rng = dist(Globals::urng);
    for(Choice c : choices){
        rng-=c.second;
        if(rng<=0){
            return c.first;
        }
    }
    return choices.back().first;
}  



vector<int>& Ant::getPath(){
    return path;
}

void Ant::setPath(vector<int> path){
    this->path = path;
}