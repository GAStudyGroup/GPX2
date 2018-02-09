#include "Opt.hpp"

#include <iostream>
#include <algorithm>

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
        for(unsigned i=0;i<routeSize;i++){
            edge baseEdge(i,((i+1)%routeSize));

            for(unsigned j=0;j<routeSize;j++){
                edge comparingEdge(((j+i+1)%routeSize),((j+i+2)%routeSize));

                if(!obj.adjacent(baseEdge,comparingEdge)){
                    
                    if(obj.isBetter(baseEdge,comparingEdge,tour)){
                        /* std::cout<<"--------------------------------------\nbase edge: "<<baseEdge.first<<"->"<<baseEdge.second<<std::endl;
                        std::cout<<"\tcomparing edge: "<<comparingEdge.first<<"->"<<comparingEdge.second<<std::endl;
                    
                        std::cout<<"better"<<std::endl; */
                        /* reverse(find(tour.getRoute().begin(),tour.getRoute().end(),tour.getRoute()[comparingEdge.first]),find(tour.getRoute().begin(),tour.getRoute().end(),baseEdge.second)); */
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

    /* bool run{false};

    //transformar em map
    obj.map = tourToMap(tour);
    
    do{
        vector<string> alreadyVisited;
        deque<string> nextToVisit;
        nextToVisit.push_back((*obj.map.begin()).first);
        while(!nextToVisit.empty()){
            alreadyVisited.push_back(nextToVisit.back());
            string baseNode = nextToVisit.back(),edgeNode;
            for(pair<string,double> node : (*obj.map[nextToVisit.back()]).getEdges()){
                if(find(alreadyVisited.begin(),alreadyVisited.end(),node.first)==alreadyVisited.end()){
                    nextToVisit.push_back(node.first);
                    edgeNode = node.first;
                    break;
                }
            }
            nextToVisit.pop_front();
            if(!nextToVisit.empty()){
                edge baseEdge((*obj.map[baseNode]),(*obj.map[edgeNode]));
                std::cout<<"base edge "<<baseEdge.first.getId()<<"->"<<baseEdge.second.getId()<<std::endl;
                {
                    vector<string> alreadyVisitedInside;
                    deque<string> nextToVisitInside;
                    alreadyVisitedInside.push_back(baseEdge.first.getId());
                    nextToVisitInside.push_back(baseEdge.second.getId());
                    while(!nextToVisitInside.empty()){
                        alreadyVisitedInside.push_back(nextToVisitInside.back());
                        string comparingFirstNode = nextToVisitInside.back(), comparingSecondNode;
                        for(pair<string,double> node : (*obj.map[nextToVisitInside.back()]).getEdges()){
                            if(find(alreadyVisitedInside.begin(),alreadyVisitedInside.end(),node.first)==alreadyVisitedInside.end()){
                                nextToVisitInside.push_back(node.first);
                                comparingSecondNode = node.first;
                                break;
                            }
                        }

                        nextToVisitInside.pop_front();
                        if(!nextToVisitInside.empty()){
                            edge comparingEdge((*obj.map[comparingFirstNode]),(*obj.map[comparingSecondNode]));
                            std::cout<<"Comparing with "<<comparingEdge.first.getId()<<"->"<<comparingEdge.second.getId()<<std::endl;
                            if(!obj.adjacent(baseEdge,comparingEdge)){
                                if(obj.isBetter(baseEdge,comparingEdge)){
                                    std::cout<<"changing edges"<<std::endl;
                                }
                            }else{
                                std::cout<<"adj"<<std::endl;
                            }
                        }
                    }
                }
            }
        }
    }while(run);

    //retournar para tour
    return(mapToTour(obj.map)); */
}

/* bool Opt::adjacent(edge &e1, edge &e2){
    if(!e1.first.getId().compare(e2.first.getId()) || !e1.first.getId().compare(e2.second.getId()) || !e1.second.getId().compare(e2.first.getId()) || !e1.second.getId().compare(e2.second.getId())){
        //edges are adjacent
        return(true);
    }else{
        return(false);
    }
} */

/* bool Opt::isBetter(edge &e1, edge &e2){
    double  oldDistance = (distance(e1.first,e1.second) + distance(e2.first,e2.second)), newDistance = ((distance(e1.first,e2.first)) + (distance(e1.second,e2.second)));

    std::cout<<distance(e1.first,e1.second)<<"\n";
    std::cout<<distance(e2.first,e2.second)<<"\n";
    std::cout<<distance(e1.first,e2.first)<<"\n";
    std::cout<<distance(e1.second,e2.second)<<"\n";

    std::cout<<"oldDistance "<<oldDistance<<", newDistance "<<newDistance<<std::endl;
    if(newDistance<oldDistance){
        return(true);
    }else{
        return(false);
    }
} */

bool Opt::adjacent(edge &e1, edge &e2){
    if(e1.first==e2.first || e1.first==e2.second || e1.second==e2.first || e1.second==e2.second){
        return(true);
    }else{
        return(false);
    }
}

bool Opt::isBetter(edge &e1, edge &e2, Tour &t){
    int oldDistance = t.distance(e1.first,e1.second) + t.distance(e2.first,e2.second);
    int newDistance = t.distance(e1.first,e2.first) + t.distance(e1.second,e2.second);
    if(oldDistance>newDistance){
        return(true);
    }else{
        return(false);
    }
}