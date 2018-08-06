#include "OX.hpp"

#include "Globals.hpp"

#include <random>
using std::uniform_int_distribution;

#include <algorithm>
using std::find;

#include <iostream>
using std::cout;
using std::endl;

#include "Utils.hpp"

vector<int> OX::crossover(vector<int> t1, vector<int> t2){
    uniform_int_distribution<int> dist(0,Globals::TOUR_SIZE-1);
    vector<int> tmp1,tmp2;
    unsigned cut1, cut2;
    
    do{
        cut1 = (unsigned)dist(Globals::urng);
        cut2 = (unsigned)dist(Globals::urng);
        if(cut2<cut1){
            unsigned tmp{cut2};
            cut2 = cut1;
            cut1 = tmp;
        }
        // cout<<"cut1 "<<cut1<<" cut2 "<<cut2<<endl;
    }while(cut1==cut2);


    for (unsigned i{ cut1 }; i <= cut2; i++) {
        tmp2.push_back(t1[i]);
        tmp1.push_back(t2[i]);
    }

    // cout<<"tmp"<<endl;
    // for(int i : tmp1){
    //     cout<<i<<" ";
    // }cout<<endl;

    // for(int i : tmp2){
    //     cout<<i<<" ";
    // }cout<<endl;

    // for(unsigned i=0;i<Globals::TOUR_SIZE;i++){
    //     if(find(tmp1.begin(),tmp1.end(),t1[i])!=tmp1.end()){
    //         t1[i] = -1;
    //     }
    //     if(find(tmp2.begin(),tmp2.end(),t2[i])!=tmp2.end()){
    //         t2[i] = -1;
    //     }
    // }

    for(unsigned i=0;i<tmp1.size();i++){
        t1[find(t1.begin(),t1.end(),tmp1[i]) - t1.begin()] = -1;
        t2[find(t2.begin(),t2.end(),tmp2[i]) - t2.begin()] = -1;
    }

    // cout<<"end"<<endl;
    // for(int i : t1){
    //     cout<<i<<" ";
    // }cout<<endl;

    // for(int i : t2){
    //     cout<<i<<" ";
    // }cout<<endl;

    for(unsigned i=0;i<Globals::TOUR_SIZE;i++){
        if(t1[i]!=-1){
            tmp1.push_back(t1[i]);
        }
        if(t2[i]!=-1){
            tmp2.push_back(t2[i]);
        }
    }

    return getFitness(tmp1)<getFitness(tmp2)?tmp1:tmp2;
};