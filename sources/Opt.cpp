#include "Opt.hpp"

#include <iostream>
#include <algorithm>
#include <utility> 

using std::find;
using std::reverse;

Opt::Opt(){}

Opt::~Opt(){
    /* deleteCityMap(map); */
}

Tour Opt::optimize(Tour &tour){
    Opt obj;

    bool run{false};
    long unsigned int routeSize{tour.getRoute().size()};
    
    do{
        run = false;
        for(unsigned i=1;i<routeSize-2;i++){
            edge baseEdge(i-1,i);

            for(unsigned j=i+1;j<routeSize-1;j++){
                edge comparingEdge(j,j+1);

                if(!obj.adjacent(baseEdge,comparingEdge)){
                    
                    if(obj.isBetter(baseEdge,comparingEdge,tour)){
                        reverse(tour.getRoute().begin()+baseEdge.second,tour.getRoute().begin()+comparingEdge.first+1);
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

bool Opt::isBetter(edge &e1, edge &e2, Tour &t){
    int oldDistance = distance(std::make_pair(t.getRoute()[e1.first].getX(),t.getRoute()[e1.first].getY()),std::make_pair(t.getRoute()[e1.second].getX(),t.getRoute()[e1.second].getY())) 
    + distance(std::make_pair(t.getRoute()[e2.first].getX(),t.getRoute()[e2.first].getY()),std::make_pair(t.getRoute()[e2.second].getX(),t.getRoute()[e2.second].getY()));

    int newDistance = distance(std::make_pair(t.getRoute()[e1.first].getX(),t.getRoute()[e1.first].getY()),std::make_pair(t.getRoute()[e2.first].getX(),t.getRoute()[e2.first].getY())) 
    + distance(std::make_pair(t.getRoute()[e1.second].getX(),t.getRoute()[e1.second].getY()),std::make_pair(t.getRoute()[e2.second].getX(),t.getRoute()[e2.second].getY()));

    if(oldDistance>newDistance){
        return(true);
    }else{
        return(false);
    }
}