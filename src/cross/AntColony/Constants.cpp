#include "Constants.hpp"

#include "Globals.hpp"

#include <algorithm>
using std::find;

#include <iostream>
using std::cout;
using std::endl;

#include "Utils.hpp"

#include <cmath>
using std::pow;

double AntConstants::alfa = 2.0;
double AntConstants::beta = 6.0;
double AntConstants::Q = 1.0;
double AntConstants::rho = 0.02;
double AntConstants::Pbest = 0.005;

double **AntMap::pheromoneMap = nullptr;
int AntMap::bestFitnessYet = 0;
Ant AntMap::bestAntYet;

//the first city in the TSP problem is 1 not 0
double AntMap::getPheromoneLevel(int a,int b){
    return AntMap::pheromoneMap[a-1][b-1];
}

void AntMap::setPheromoneLevel(int a, int b, double newValue){
    AntMap::pheromoneMap[a-1][b-1] = newValue;
}


void AntMap::updatePheromoneMap(const vector<int>& path){
    for(unsigned i=1;i<=Globals::TOUR_SIZE;i++){
        for(unsigned j=1;j<=Globals::TOUR_SIZE;j++){
            if(i!=j){
                double decay{((1-AntConstants::rho)*AntMap::getPheromoneLevel(i,j))},
                    add{(constainsEdge(path,i,j)?(AntConstants::Q/getFitness(path)):(0.0))},
                    max{maxPheromoneLevel()},
                    min{minPheromoneLevel()},
                    newValue{decay+add};
                
                if(min>max){
                    min = max;
                }
                
                if(newValue > max){
                    newValue = max;
                }else if(newValue < min){
                    newValue = min;
                }
                cout<<"maxPheromoneLevel: "<<max<<" minPheromoneLevel: "<<min<<endl;
                AntMap::setPheromoneLevel(i,j,newValue);
                //because assymetric TSP
                AntMap::setPheromoneLevel(j,i,newValue);
            }
            
        }   
    }
}

bool AntMap::constainsEdge(const vector<int>& path,int a, int b){
    //find a position in ant`s path
    long int aIndex{find(path.begin(),path.end(),a)-path.begin()};
    //if b is next in position them ant path contains the AB edge
    return path[(aIndex+1)%Globals::TOUR_SIZE]==b;
}


void AntMap::printPheromoneMap(){
    for(unsigned i=1;i<=Globals::TOUR_SIZE;i++){
        for(unsigned j=1;j<=Globals::TOUR_SIZE;j++){
            cout<<AntMap::getPheromoneLevel(i,j)<<" ";
        }
        cout<<"\n";
    }
    cout<<endl;
}

double AntMap::maxPheromoneLevel(){
    return AntConstants::Q/(AntConstants::rho*bestFitnessYet);
}

double AntMap::minPheromoneLevel(){
    //here Globals::TOUR_SIZE is used as the number of ants
    return (maxPheromoneLevel()*(1-pow(AntConstants::Pbest,(1/Globals::TOUR_SIZE)))/((Globals::TOUR_SIZE/2)-1));
}
