#include "MOC.hpp"

#include <random>
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

#include <algorithm>
using std::find;
using std::find_if;

#include "Utils.hpp"

#include <iostream>
using std::endl;
using std::cout;

vector<int> MOC::cross(vector<int> red, vector<int> blue){
    //Generate random number to determine where to cut
    random_device rng;
    mt19937 urng(rng());
    uniform_int_distribution<int> dist(0,red.size()-1);
    unsigned cut = dist(urng);
    /* cout<<"cut "<<cut<<endl;
    cout<<"Red "<<getFitness(red)<<endl;
    cout<<"Blue "<<getFitness(blue)<<endl; */

    int token = -1;

    auto isToken = [token](int i){
        return(i==token);
    };

    vector<int> redTail(red.begin()+cut,red.end()),blueTail(blue.begin()+cut,blue.end());

    vector<int> offspringR,offspringB;

    for(int i=0;i<red.size();i++){
        if(find(blueTail.begin(),blueTail.end(),red[i])==blueTail.end()){
            offspringR.push_back(red[i]);
        }else{
            offspringR.push_back(-1);
        }

        if(find(redTail.begin(),redTail.end(),blue[i])==redTail.end()){
            offspringB.push_back(blue[i]);
        }else{
            offspringB.push_back(-1);
        }
    }

    /* cout<<"Offspring Red depois de corte"<<endl;
    for(int s : offspringR){
        cout<<s<<" ";
    }
    cout<<endl;
    cout<<"Offspring Blue depois de corte"<<endl;
    for(int s : offspringB){
        cout<<s<<" ";
    }
    cout<<endl; */

    for(int i=0;i<redTail.size();i++){
        vector<int>::iterator it = find_if(offspringR.begin(),offspringR.end(),isToken);
        *it = blueTail[i];

        it = find_if(offspringB.begin(),offspringB.end(),isToken);
        *it = redTail[i];
    }

    /* cout<<"Offspring Red depois da build "<<getFitness(offspringR)<<endl;
    for(int s : offspringR){
        cout<<s<<" ";
    }
    cout<<endl;
    cout<<"Offspring Blue depois da build "<<getFitness(offspringB)<<endl;
    for(int s : offspringB){
        cout<<s<<" ";
    }
    cout<<endl; */

    if(getFitness(offspringR)<getFitness(offspringB)){
        if(getFitness(offspringR)<getFitness(red) && getFitness(offspringR)<getFitness(blue)){
            cout<<"Red "<<getFitness(red)<<endl;
            cout<<"Blue "<<getFitness(blue)<<endl;
            cout<<"melhorou a fitness!!! "<<getFitness(offspringR)<<endl;
        }
        return offspringR;
    }else{
        if(getFitness(offspringB)<getFitness(red) && getFitness(offspringB)<getFitness(blue)){
            cout<<"Red "<<getFitness(red)<<endl;
            cout<<"Blue "<<getFitness(blue)<<endl;
            cout<<"melhorou a fitness!!! "<<getFitness(offspringB)<<endl;
        }
        return offspringB;
    }
}