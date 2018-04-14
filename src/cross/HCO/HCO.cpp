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
    cout<<"entrando \nred: "<<getFitness(red)<<"\nblue: "<<getFitness(blue);
    // cout<<"=======================================================================================================================\n\ncomeçando "<<size<<endl;
    vector<int> offspring,tmpDist{red};
    uniform_int_distribution<int> dist(0,size-1);

    int begin{red[dist(Globals::urng)]};

    unsigned redPos{find(red.begin(),red.end(),begin)-red.begin()};
    rotate(red.rbegin(),red.rend()-redPos,red.rend());

    unsigned bluePos{find(blue.begin(),blue.end(),begin)-blue.begin()};
    rotate(blue.rbegin(),blue.rend()-bluePos,blue.rend());

    offspring.push_back(begin);
    tmpDist.erase(remove(tmpDist.begin(),tmpDist.end(),begin),tmpDist.end());

    /* cout<<"begin "<<begin<<endl;

    cout<<"red"<<endl;
    for(int i : red){
        cout<<i<<" ";
    }
    cout<<endl;

    cout<<"blue"<<endl;
    for(int i : blue){
        cout<<i<<" ";
    }
    cout<<endl;

    cout<<"offspring"<<endl;
    for(int i : offspring){
        cout<<i<<" ";
    }
    cout<<endl;

    cout<<"tmpDist"<<endl;
    for(int i : tmpDist){
        cout<<i<<" ";
    }
    cout<<endl;
    cout<<endl;
    cout<<endl; */

    for(unsigned i = 1;i<size;i++){
        /* if(distance(red[i-1],red[i])<distance(blue[i-1],blue[i])){
            cout<<"pushing red "<<red[i]<<endl;
            offspring.push_back(red[i]);

            unsigned bluePos{find(blue.begin(),blue.end(),offspring[i])-blue.begin()};
            rotate(blue.begin()+i,blue.begin()+bluePos,blue.end());

        }else{
            cout<<"pushing blue "<<blue[i]<<endl;
            offspring.push_back(blue[i]);

            unsigned redPos{find(red.begin(),red.end(),offspring[i])-red.begin()};
            rotate(red.begin()+i,red.begin()+redPos,red.end());

            } */

        if(isNeighbor(redOrig,red[i],red[i-1]) && isNeighbor(blueOrig,blue[i],blue[i-1])){
            if(distance(red[i-1],red[i])<distance(blue[i-1],blue[i])){
                // cout<<"pushing red "<<red[i]<<endl;
                offspring.push_back(red[i]);

                unsigned bluePos{find(blue.begin(),blue.end(),offspring.back())-blue.begin()};
                rotate(blue.begin()+i,blue.begin()+bluePos,blue.end());

            }else{
                // cout<<"pushing blue "<<blue[i]<<endl;
                offspring.push_back(blue[i]);

                unsigned redPos{find(red.begin(),red.end(),offspring.back())-red.begin()};
                rotate(red.begin()+i,red.begin()+redPos,red.end());

            }
        }else{
            sort(tmpDist.begin(),tmpDist.end(),[offspring](int a,int b){
                return(distance(a,offspring.back()) < distance(b,offspring.back()));
            });
            
            // cout<<"pushing tmpDist "<<tmpDist[0]<<endl;
            offspring.push_back(tmpDist[0]);

            unsigned bluePos{find(blue.begin(),blue.end(),offspring.back())-blue.begin()};
            rotate(blue.begin()+i,blue.begin()+bluePos,blue.end());

            unsigned redPos{find(red.begin(),red.end(),offspring.back())-red.begin()};
            rotate(red.begin()+i,red.begin()+redPos,red.end());

        }
        tmpDist.erase(remove(tmpDist.begin(),tmpDist.end(),offspring.back()),tmpDist.end());

        /* cout<<"red"<<endl;
        for(int i : red){
            cout<<i<<" ";
        }
        cout<<endl;

        cout<<"blue"<<endl;
        for(int i : blue){
            cout<<i<<" ";
        }
        cout<<endl;

        cout<<"offspring"<<endl;
        for(int i : offspring){
            cout<<i<<" ";
        }
        cout<<endl;

        cout<<"tmpDist"<<endl;
        for(int i : tmpDist){
            cout<<i<<" ";
        }
        cout<<endl;
        cout<<endl;
        cout<<endl; */
    }
    // cout<<"saindo "<<offspring.size()<<endl;
    // exit(0);
    cout<<"\nsaindo\nOffspring: "<<getFitness(offspring)<<"\n"<<endl;
    return offspring;
}

bool HCO::isNeighbor(const vector<int> tour,const int id,const int next){
    int index{find(tour.begin(),tour.end(),id)-tour.begin()};
    unsigned size{tour.size()};
    /* cout<<"====================\n\ncomparing tour "<<endl;

    for_each(tour.begin(),tour.end(),[](int i){
        cout<<i<<" ";
    });
    cout<<endl;

    cout<<"comparing "<<id<<" with "<<next<<endl; */

    if(tour[(index-1)%size]==next){
        return true;
    }else if(tour[(index+1)%size]==next){
        return true;
    }else{
        return false;
    }
}