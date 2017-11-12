#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include <fstream>

#include "Map.hpp"
#include "Population.hpp"
#include "ImportData.hpp"
#include "GPX2.hpp"

using std::cout;
using std::endl;
using std::stoi;
using std::invalid_argument;
using std::random_shuffle;
using std::srand;

void GA(string,unsigned,int);
bool stop(Population);
Population generateNewPopulation(Population);
std::ofstream file;
std::ofstream fileDebug;


//primeiro argumento tour_name, segundo tamanho da pop
int main(int argc, char *argv[]){
    string name{""};
    unsigned popSize{0};
    int id{0};
    
    //srand(time(NULL));

    if(argc == 4){
        try{
            name = argv[1];
            popSize = stoi(argv[2]);
            id = stoi(argv[3]);
        }catch(invalid_argument &i_a){
            cout<<"Invalid population size! "<<i_a.what()<<endl;
            return(0);
        }
    }else if(argc == 0){
        cout<<"Please specify the arguments: tour name and population size."<<endl;
        return(0);
    }else{
        cout<<"Invalid argument number. Expecting two arguments."<<endl;
        return(0);
    }

    GA(name,popSize,id);

    return 0;
}

void GA(string name,unsigned popSize,int id){

    Map map;
    Population pop;

    {
        ImportData dataFile(name);
        //carrega o mapa
        map.setCityList(dataFile.getCitiesCoord());

        //carrega a primeira população
        //pop = dataFile.importFirstPopulation(map,name,popSize);
        pop = dataFile.importFirstPopulation(map,"log",popSize);

    }

    /* for(Tour t : pop.getPopulation()){
        cout<<t<<"\n";
    }
    cout.flush(); */
    
    
/*     file.open("log_"+to_string(id)+".txt");
    fileDebug.open("log_"+to_string(id)+"_BUG.txt");

    int i{0},firstBestFitness{pop.bestFitness()};
    while(stop(pop)){
        file<<"BestFitness: "<<pop.bestFitness()<<endl;

        pop = generateNewPopulation(pop);

        if(i%10 == 0){
            cout<<"gen "<<i<<" best fitness "<<pop.bestFitness()<<endl;
        }
        i++;
    }
    cout<<"THE END"<<endl;
    cout<<"first best fitness: "<<firstBestFitness<<endl;
    cout<<"gen "<<i<<" best fitness "<<pop.bestFitness()<<endl;
    cout << "========================="<<endl; */

    /* cout<<"Antes"<<endl;
    for(auto t : pop.getPopulation()){
        cout << t.getFitness() << endl;
    }
 */
    cout<<"before"<<endl;
    for(Tour t : pop.getPopulation()){
        cout<<t<<endl;
    }

    pop = generateNewPopulation(pop);

    cout<<"after"<<endl;
    for(Tour t : pop.getPopulation()){
        cout<<t<<endl;
    }

    /* cout<<"Depois"<<endl;
    for(auto t : pop.getPopulation()){
        cout << t.getFitness() << endl;
    } */

    fileDebug.flush();
    fileDebug.close();
    file.flush();
    file.close(); 

}

bool stop(Population pop){

    int static generationsWithoutChange{0};
    int static bestFitness{pop.bestFitness()};
    int currentFitness{pop.bestFitness()};
    
    unsigned totalCon{0};
    for(Tour t : pop.getPopulation()){
        if(t.getFitness()==bestFitness){
            totalCon++;
        }
    }


    if(bestFitness > currentFitness){
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
        //cout<<"new best fitness: "<<bestFitness<<endl;
    }else if(totalCon==pop.getPopulation().size()){
        cout<<"all the tours are the same"<<endl;
        return(false);
    }else{
        generationsWithoutChange++;
    }
    //cout<<"tours that are the same "<<totalCon<<endl;
    //file<<"SameTours: "<<totalCon<<endl;
    if(generationsWithoutChange == 100){
        return(false);
    }else{
        return(true);
    }
}

Population generateNewPopulation(Population pop){
    unsigned size = pop.getPopulation().size();
    Population newPop;

    std::random_shuffle(pop.getPopulation().begin(),pop.getPopulation().end());
    
    for(unsigned i=0;i<size;i++){
        //file << "CROSSOVER: ";
        if(i==(size-1)){
            
            newPop.addNewTour(GPX2::crossover(pop.getPopulation().at(i),pop.getPopulation().at(0)));
            /* file<<pop.getPopulation().at(i).getFitness()<<" "<<pop.getPopulation().at(0).getFitness()<<" "<<newPop.getPopulation().back().getFitness()<<endl;
            if(pop.getPopulation().at(i).getFitness() < newPop.getPopulation().back().getFitness() || pop.getPopulation().at(0).getFitness() < newPop.getPopulation().back().getFitness()){
                fileDebug<<pop.getPopulation().at(i)<<endl;
                fileDebug<<"\n\n\n\n\n\n"<<endl;
                fileDebug<<pop.getPopulation().at(0)<<endl;
            } */
            cout<<"before RED: "<<pop.getPopulation().at(i).getFitness()<<" BLUE: "<<pop.getPopulation().at(0).getFitness()<<endl;
            cout<<"after OFFSPRING: "<<newPop.getPopulation().back().getFitness()<<endl;
        }else{
            
            newPop.addNewTour(GPX2::crossover(pop.getPopulation().at(i),pop.getPopulation().at(i+1)));
            /* file<<pop.getPopulation().at(i).getFitness()<<" "<<pop.getPopulation().at(i+1).getFitness()<<" "<<newPop.getPopulation().back().getFitness()<<endl;
            if(pop.getPopulation().at(i).getFitness() < newPop.getPopulation().back().getFitness() || pop.getPopulation().at(i+1).getFitness() < newPop.getPopulation().back().getFitness()){
                fileDebug<<pop.getPopulation().at(i)<<endl;
                fileDebug<<"\n\n\n\n\n\n"<<endl;
                fileDebug<<pop.getPopulation().at(i+1)<<endl;
            } */
            cout<<"before RED: "<<pop.getPopulation().at(i).getFitness()<<" BLUE: "<<pop.getPopulation().at(i+1).getFitness()<<endl;
            cout<<"after OFFSPRING"<<newPop.getPopulation().back().getFitness()<<endl;
        }
    }

    return newPop;
}