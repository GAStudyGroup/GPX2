#include "Population.hpp"

Population::Population(){}

vector<Tour>& Population::getPopulation(){ return(population); }

void Population::addNewTour(Tour t){ population.push_back(t); }

double Population::bestFitness(){
    double max = -1*numeric_limits<double>::max(),fitness{0.0};
    for(Tour t : population){
        fitness = t.getFitness();
        if(max < fitness){
            max = fitness;
        }
    }
    return(max);
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

