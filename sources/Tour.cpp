#include "Tour.hpp"

Tour::Tour(){}

Tour::Tour(vector<City> route):route(route){}

void Tour::setRoute(vector<City> route){
    this->route = route;
}

vector<City>& Tour::getRoute(){
    return route;
}

double Tour::getFitness(){  // Irá retornar a fitness do Tour específico
    double sum{0.0};
    for(unsigned i=0;i<(route).size();i++){
        if((i+1) == (route).size()){
            sum+=distance(i,0);
        }else{
            sum+=distance(i,i+1);
        }
    }
    return(sum); 
}

double Tour::distance(const int a,const int b){  // Retorno da distancia entre duas cidades adjacentes
    return(sqrt(pow(((route)[a].getX())-((route)[b].getX()),2)+pow(((route)[a].getY())-((route)[b].getY()),2)));
}

ostream& operator<<(ostream &output,Tour &t){  // Overload de operador para impressão da população
    
    for(City c : t.getRoute()){
        output<<"\t"<<c<<endl;
        
    }
    output<<"Fitness: "<<t.getFitness()<<endl;
    //output<<"Distance: "<<t.dist<<endl;
    return(output);
}