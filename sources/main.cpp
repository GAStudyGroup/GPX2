#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <chrono>
#include <stdexcept>

#include "Config.hpp"
#include "Population.hpp"
#include "Utils.hpp"
#include "GAUtils.hpp"
#include "Arg.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::srand;
using std::stof;
using std::stoi;
using std::ofstream;

// inicia o algoritmo genético
void GA();

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

int main(int argc, char *argv[]) {
    srand(time(NULL));

    string NAME = "name";
    string SIZE = "size";
    string LIB = "lib";
    string ID = "id";
    string LK = "lk";
    string NEW_POP = "newpop";
    string N_BEST = "nbest";
    string BEST_FITNESS = "bestfitness";

    Arg arg(argc,argv);

    arg.newArgument(NAME,true,"n");
    arg.newArgument(SIZE,true,"s");
    arg.newArgument(LIB,false,"l");
    arg.newArgument(ID,false);
    arg.newArgument(LK,false);
    arg.newArgument(NEW_POP,false,"np");
    arg.newArgument(N_BEST,false,"nb");
    arg.newArgument(BEST_FITNESS,false,"bf");
    
    try{
        arg.validateArguments();
    }catch(std::runtime_error e){
        std::cout<<e.what()<<endl;
        return(0);
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

    GA();

    return 0;
}

void GA() {
    Population pop;

    ofstream *logFile{GAUtils::initLogFile()};  

    GAUtils::printHeader(*logFile);

    auto start = std::chrono::high_resolution_clock::now();

    GAUtils::init(pop);

    auto finishInitPop = std::chrono::high_resolution_clock::now();

    GAUtils::printTime(*logFile,"Population created in:",
                    std::chrono::duration<double,std::milli> (finishInitPop - start).count(),
                    std::chrono::duration<double> (finishInitPop - start).count()); 

    auto startGA = std::chrono::high_resolution_clock::now();

    int i{1}, firstBestFitness{pop.bestFitness()};
    *logFile << "\nFirst fitness " << firstBestFitness << endl;

    do {
        pop = GAUtils::generateNewPopulation(pop);
        *logFile << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        i++;
    } while (GAUtils::stop(pop,*logFile));

    auto finishGA = std::chrono::high_resolution_clock::now();
    GAUtils::printTime(*logFile,"GA execution time:",
                    std::chrono::duration<double,std::milli> (finishGA - startGA).count(),
                    std::chrono::duration<double> (finishGA - startGA).count()); 

    auto finish = std::chrono::high_resolution_clock::now();
    GAUtils::printTime(*logFile,"Total execution time:",
                    std::chrono::duration<double,std::milli> (finish - start).count(),
                    std::chrono::duration<double> (finish - start).count()); 

    GAUtils::printFooter(*logFile,pop,i,firstBestFitness);

    (*logFile).close();
    delete logFile;

}