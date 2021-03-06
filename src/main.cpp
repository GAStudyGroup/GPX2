#include <random>
using std::random_device;

#include <fstream>
using std::ofstream;

#include <iostream>
using std::cout;
using std::endl;

#include <chrono>
//chrono

#include <vector>
using std::vector;

#include <stdexcept>
using std::invalid_argument; 

#include <string>
using std::stof;
using std::stoi;

#include "Globals.hpp"
#include "Population.hpp"
#include "GAUtils.hpp"
#include "Log.hpp"
#include "Arg.hpp"

#include "Constants.hpp"

//begin the genetic algorithm
void GA();

//set arguments
void initArgs(int, char *[]);

//1 -name argumento tour_name REQUIRED
//2 -lib caminho para o .tsp
//3 -size tamanho da pop REQUIRED
//4 -id ID da run(usado para log
//5 -lk porcentagem de população inicial gerado pelo LK 
//6 -newpop tipo de geração da nova poop
//7 -nbest n best para serem salvos para a proxima geração
//8 -bestfitness best fitness conhecida
// ex: GA -n berlin52 -l lib/ -s 100 -id 3 -lk 0.1 -np 1 -nb 4 -bf 255 
// ex: GA -name berlin52 -size 100

#include "OX.hpp"

int main(int argc, char *argv[]) {
    random_device rng;
    Globals::urng.seed(rng());

    // vector<int> t1,t2;
    // t1.push_back(1);
    // t1.push_back(2);
    // t1.push_back(3);
    // t1.push_back(4);
    // t1.push_back(5);
    // t1.push_back(6);

    // t2.push_back(5);
    // t2.push_back(6);
    // t2.push_back(4);
    // t2.push_back(2);
    // t2.push_back(1);
    // t2.push_back(3);
    // Globals::TOUR_SIZE = t1.size();

    // cout<<"chegou aqui?"<<endl;

    // OX::crossover(t1,t2);

    // for(int i : t1){
    //     cout<<i<<" ";
    // }cout<<endl;

    // for(int i : t2){
    //     cout<<i<<" ";
    // }cout<<endl;

    initArgs(argc,argv);

    GA();

    return 0;
}

void GA() {
    Population pop;
    bool foundBestWithoutGA = true;

    ofstream *logFile{Log::initLogFile()};  

    Log::printHeader(*logFile);

    auto start = std::chrono::high_resolution_clock::now();

    GAUtils::init(pop);
    AntMap::initMap();
    AntMap::updatePheromoneMap(pop.getBestTour());

    // cout<<"tour size "<<Globals::TOUR_SIZE<<endl;
    // antColony.printPheromoneMap();

    // Population tmpPop = antColony.run();
    // cout<<"Size of return pop "<<tmpPop.getPopulation().size()<<" Best fitness of run 0 "<<tmpPop.bestFitness()<<endl;
    // antColony.printPheromoneMap();

    auto finishInitPop = std::chrono::high_resolution_clock::now();

    Log::printTime(*logFile,"Population created in:",
                    std::chrono::duration<double,std::milli> (finishInitPop - start).count()); 

    auto startGA = std::chrono::high_resolution_clock::now();

    int i{0}, firstBestFitness{pop.bestFitness()};
    *logFile << "\nFirst fitness " << firstBestFitness << endl;
    while(GAUtils::stop(pop,*logFile)){
        foundBestWithoutGA = false;
        i++;
        pop = GAUtils::generateNewPopulation(pop);
        // AntMap::printPheromoneMap();
        *logFile << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        
    }

    if(foundBestWithoutGA){
        *logFile << "Found best without GA" << endl;
        cout << "Found best without GA" << endl;
    }

    auto finishGA = std::chrono::high_resolution_clock::now();
    Log::printTime(*logFile,"GA execution time:",
                    std::chrono::duration<double,std::milli> (finishGA - startGA).count()); 

    auto finish = std::chrono::high_resolution_clock::now();
    Log::printTime(*logFile,"Total execution time:",
                    std::chrono::duration<double,std::milli> (finish - start).count()); 

    Log::printFooter(*logFile,pop,i,firstBestFitness);

    (*logFile).close();
    delete logFile;
    AntMap::deleteMap();
}

void initArgs(int argc, char *argv[]){
    string NAME = "name|n";
    string SIZE = "size|s";
    string LIB = "lib|l";
    string ID = "id";
    string LK = "lk";
    string NEW_POP = "newpop|np";
    string N_BEST = "nbest|nb";
    string BEST_FITNESS = "bestfitness|bf";
    string RESET = "reset|r";
    string GEN_NEW_TOUR = "newtour|nt";

    Arg arg(argc,argv);
    arg.setProgramName("GPX2");
    arg.setHelp();

    arg.newArgument(NAME,true,"name of the tour.");
    arg.newArgument(SIZE,true,"size of the pop.");
    arg.newArgument(LIB,false,"path to the .tsp file.");
    arg.newArgument(ID,false,"numeric id of the run.");
    arg.newArgument(LK,false,"percentage of population to be generated using linkern.");
    arg.newArgument(NEW_POP,false,"method to be used to generate the next generation.");
    arg.newArgument(N_BEST,false,"number of tours to be saved to the next generation.");
    arg.newArgument(BEST_FITNESS,false,"best fitness found to this tour.");
    arg.newArgument(RESET,false,"percentage of the population to reset each generation");
    arg.newArgument(GEN_NEW_TOUR,false,"mode to generate new tour, used to generate first population and reset");

    try{
        arg.validateArguments();
    }catch(std::runtime_error &e){
        std::cout<<e.what()<<endl;
        exit(0);
    }

    
    Config::NAME = arg.getOption(NAME);
    Config::POP_SIZE = stoi(arg.getOption(SIZE));
    string tmp = arg.getOption(LIB);
    Config::LIB_PATH = tmp.empty()?Config::LIB_PATH:tmp;
    tmp = arg.getOption(ID);
    Config::ID = tmp.empty()?Config::ID:stoi(tmp);
    tmp = arg.getOption(LK);
    Config::LK_PERCENTAGE = tmp.empty()?Config::LK_PERCENTAGE:stod(tmp);
    tmp = arg.getOption(NEW_POP);
    Config::NEW_POP_TYPE = tmp.empty()?Config::NEW_POP_TYPE:stoi(tmp);
    tmp = arg.getOption(N_BEST);
    Config::N_BEST = tmp.empty()?Config::N_BEST:stoi(tmp);
    tmp = arg.getOption(BEST_FITNESS);
    Config::BEST_FITNESS = tmp.empty()?Config::BEST_FITNESS:stoi(tmp);
    tmp = arg.getOption(RESET);
    Config::RESET_PERCENTAGE = tmp.empty()?Config::RESET_PERCENTAGE:stod(tmp);
    tmp = arg.getOption(GEN_NEW_TOUR);
    Config::NEW_TOUR_MODE = tmp.empty()?Config::NEW_TOUR_MODE:stod(tmp);
}