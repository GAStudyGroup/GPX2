#include "Utils.hpp"

double getFitness(vector<int> tour, unordered_map<int,pair<double,double>> &map){
    double fitness{0.0};
    for(unsigned i=0;i<tour.size();i++){
        if(i<(tour.size()-1)){
            fitness+=distance(tour[i],tour[i+1],map);
        }else{
            fitness+=distance(tour[i],tour[0],map);
        }
    }
    return(fitness);
}

//EUCLID_2D
double distance(int c1,int c2, unordered_map<int,pair<double,double>> &map){
    return(round(sqrt(pow((map[c1].first - map[c2].first), 2) + pow((map[c1].second - map[c2].second), 2))));
}