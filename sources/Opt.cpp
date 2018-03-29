#include "Opt.hpp"

#include <iostream>
#include <algorithm>
#include <utility> 

#include "Config.hpp"

using std::find;
using std::reverse;

Opt::Opt(){}

Opt::~Opt(){
    /* deleteCityMap(map); */
}

vector<int> Opt::optimize(vector<int> &tour){
    Opt obj;

    bool run{false};
    long unsigned int routeSize{tour.size()};
    do{
        run = false;
        for(unsigned i=1;i<routeSize-2;i++){
            edge baseEdge(i-1,i);
            for(unsigned j=i+1;j<routeSize-1;j++){
                edge comparingEdge(j,j+1);
                if(!obj.adjacent(baseEdge,comparingEdge)){
                    if(obj.isBetter(baseEdge,comparingEdge,tour)){
                        reverse(tour.begin()+baseEdge.second,tour.begin()+comparingEdge.first+1);
                        run = true;
                        break;
                    }
                }
            }
            if(run){
                break;
            }
        }
    }while(run);

    return(tour);
}

bool Opt::adjacent(edge &e1, edge &e2){
    if(e1.first==e2.first || e1.first==e2.second || e1.second==e2.first || e1.second==e2.second){
        return(true);
    }else{
        return(false); 
    }
}

bool Opt::isBetter(edge &e1, edge &e2, vector<int> &t){
    /* double e1_first_x{Config::map.getCityById(t.getRoute()[e1.first]).getX()},
            e1_first_y{Config::map.getCityById(t.getRoute()[e1.first]).getY()},
            e1_second_x{Config::map.getCityById(t.getRoute()[e1.second]).getX()},
            e1_second_y{Config::map.getCityById(t.getRoute()[e1.second]).getY()},
            e2_first_x{Config::map.getCityById(t.getRoute()[e2.first]).getX()},
            e2_first_y{Config::map.getCityById(t.getRoute()[e2.first]).getY()},
            e2_second_x{Config::map.getCityById(t.getRoute()[e2.second]).getX()},
            e2_second_y{Config::map.getCityById(t.getRoute()[e2.second]).getY()}; */

    int oldDistance = distance(t[e1.first],t[e1.second])
        +
        distance(t[e2.first],t[e2.second]);
    
    int newDistance = distance(t[e1.first],t[e2.first])
        +
        distance(t[e1.second],t[e2.second]);
    if(oldDistance>newDistance){
        return(true);
    }else{
        return(false);
    }
}