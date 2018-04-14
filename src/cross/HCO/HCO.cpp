#include "HCO.hpp"

#include "Globals.hpp"
#include "Utils.hpp"

#include <algorithm>
using std::find;
using std::rotate;
using std::remove;
using std::for_each;

#include <random>
using std::uniform_int_distribution;

#include <iostream>
using std::cout;
using std::endl;

vector<int> HCO::cross(vector<int> redOrig, vector<int> blueOrig){
    unsigned size{redOrig.size()};
    vector<int> red{redOrig},blue{blueOrig};
    vector<int> offspring,tmpDist{red};
    uniform_int_distribution<int> dist(0,size-1);

    int begin{red[dist(Globals::urng)]};

    unsigned redPos{find(red.begin(),red.end(),begin)-red.begin()};
    rotate(red.rbegin(),red.rend()-redPos,red.rend());

    unsigned bluePos{find(blue.begin(),blue.end(),begin)-blue.begin()};
    rotate(blue.rbegin(),blue.rend()-bluePos,blue.rend());

    offspring.push_back(begin);
    tmpDist.erase(remove(tmpDist.begin(),tmpDist.end(),begin),tmpDist.end());

    for(unsigned i = 1;i<size;i++){
        
        if(isNeighbor(redOrig,red[i],red[i-1]) && isNeighbor(blueOrig,blue[i],blue[i-1])){
            if(distance(red[i-1],red[i])<distance(blue[i-1],blue[i])){
                offspring.push_back(red[i]);

                unsigned bluePos{find(blue.begin(),blue.end(),offspring.back())-blue.begin()};
                rotate(blue.begin()+i,blue.begin()+bluePos,blue.end());

            }else{
                offspring.push_back(blue[i]);

                unsigned redPos{find(red.begin(),red.end(),offspring.back())-red.begin()};
                rotate(red.begin()+i,red.begin()+redPos,red.end());

            }
        }else{
            sort(tmpDist.begin(),tmpDist.end(),[offspring](int a,int b){
                return(distance(a,offspring.back()) < distance(b,offspring.back()));
            });
            
            offspring.push_back(tmpDist[0]);

            unsigned bluePos{find(blue.begin(),blue.end(),offspring.back())-blue.begin()};
            rotate(blue.begin()+i,blue.begin()+bluePos,blue.end());

            unsigned redPos{find(red.begin(),red.end(),offspring.back())-red.begin()};
            rotate(red.begin()+i,red.begin()+redPos,red.end());

        }
        tmpDist.erase(remove(tmpDist.begin(),tmpDist.end(),offspring.back()),tmpDist.end());
    }
    return offspring;
}

bool HCO::isNeighbor(const vector<int> tour,const int id,const int next){
    int index{find(tour.begin(),tour.end(),id)-tour.begin()};
    unsigned size{tour.size()};

    if(tour[(index-1)%size]==next){
        return true;
    }else if(tour[(index+1)%size]==next){
        return true;
    }else{
        return false;
    }
}