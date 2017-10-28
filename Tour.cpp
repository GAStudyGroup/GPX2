#include "Tour.hpp"

Tour::Tour(){}

Tour::Tour(vector<City> tour):tour(tour){}

void Tour::setTour(vector<City> tour){
    this->tour = tour;
}

vector<City>& Tour::getTour(){
    return tour;
}

double Tour::getFitness(){  // Irá retornar a fitness do Tour específico
    double sum{0.0};
    for(int i=0;i<(tour).size();i++){
        if((i+1) == (tour).size()){
            sum+=distance(i,0);
        }else{
            sum+=distance(i,i+1);
        }
    }
    return(sum); 
}

double Tour::distance(const int a,const int b){  // Retorno da distancia entre duas cidades adjacentes
    return(sqrt(pow(((tour)[a].getX())-((tour)[b].getX()),2)+pow(((tour)[a].getY())-((tour)[b].getY()),2)));
}