#include "Population.hpp"

Population::Population(){}

vector<Tour>& Population::getPopulation(){ return(population); }

void Population::addNewTour(Tour t){ population.push_back(t); }

int Population::bestFitness(){
    int min = numeric_limits<int>::max(),fitness{0};
    for(Tour t : population){
        fitness = t.getFitness();
        if(min > fitness){
            min = fitness;
        }
    }
    return(min);
}

ostream& operator<<(ostream &output,Population &pop){  // Overload de operador para impressão da população
   int i{0};
   for(Tour t : pop.getPopulation()){
       output<<"Tour "<<setfill('0')<<setw(3)<<i<<endl;
       output<<t<<endl;
       ++i;
   }
   return(output);
}

