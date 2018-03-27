#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <chrono>

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

//1 -n argumento tour_name REQUIRED
//2 -lib caminho para o .tsp
//3 -s tamanho da pop REQUIRED
//4 -id ID da run(usado para log
//5 -lk porcentagem de população inicial gerado pelo LK 
//6 -np tipo de geração da nova poop
//7 -nb n best para serem salvos para a proxima geração
//8 -bf best fitness conhecida
// ex: GA -n berlin52 -lib lib/ -s 100 -id 3 -lk 0.1 -np 1 -nb 4 -bf 255 
// ex: GA -n berlin52 -s 100

int main(int argc, char *argv[]) {
    srand(time(NULL)); 

    Arg arg(argc,argv);

    if(arg.getOption("n").empty()){
        cout<<"name of the tour is required"<<endl;
        return(0);
    }
    if(arg.getOption("s").empty()){
        cout<<"size of the population is required"<<endl;
        return(0);
    }else{
        try {
            Config::NAME = arg.getOption("n");
            Config::POP_SIZE = stoi(arg.getOption("s"));

            string tmp{""},cmd{"lib"};
            if(arg.isSet(cmd)){
                tmp = arg.getOption(cmd);
                if(tmp == ""){
                    cout<<"command "<<cmd<<" needs an argument"<<endl;
                    return(0);
                }else{
                    Config::LIB_PATH = tmp;
                }
            }
            cmd = "id";
            if(arg.isSet(cmd)){
                tmp = arg.getOption(cmd);
                if(tmp == ""){
                    cout<<"command "<<cmd<<" needs an argument"<<endl;
                    return(0);
                }else{
                    Config::ID = stoi(tmp);
                }
            }
            cmd = "lk";
            if(arg.isSet(cmd)){
                tmp = arg.getOption(cmd);
                if(tmp == ""){
                    cout<<"command "<<cmd<<" needs an argument"<<endl;
                    return(0);
                }else{
                    Config::LK_PERCENTAGE = stof(tmp);
                }
            }
            cmd = "np";
            if(arg.isSet(cmd)){
                tmp = arg.getOption(cmd);
                if(tmp == ""){
                    cout<<"command "<<cmd<<" needs an argument"<<endl;
                    return(0);
                }else{
                    Config::NEW_POP_TYPE = stoi(tmp);
                }
            }
            cmd = "nb";
            if(arg.isSet(cmd)){
                tmp = arg.getOption(cmd);
                if(tmp == ""){
                    cout<<"command "<<cmd<<" needs an argument"<<endl;
                    return(0);
                }else{
                    Config::N_BEST = stoi(tmp);
                }
            }
            cmd = "bf";
            if(arg.isSet(cmd)){
                tmp = arg.getOption(cmd);
                if(tmp == ""){
                    cout<<"command "<<cmd<<" needs an argument"<<endl;
                    return(0);
                }else{
                    Config::BEST_FITNESS = stoi(tmp);
                }
            }
        } catch (invalid_argument &i_a) {
            cout << "Invalid argument!" << i_a.what() << endl;
            return (0);
        }
    }

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

    GAUtils::printTime(*logFile,"Population created in:",std::chrono::duration<double,std::milli> (finishInitPop - start).count(),std::chrono::duration<double> (finishInitPop - start).count()); 

    auto startGA = std::chrono::high_resolution_clock::now();

    int i{1}, firstBestFitness{pop.bestFitness()};
    *logFile << "\nFirst fitness " << firstBestFitness << endl;
    do {
        pop = GAUtils::generateNewPopulation(pop);
        *logFile << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        i++;
    } while (GAUtils::stop(pop));

    auto finishGA = std::chrono::high_resolution_clock::now();
    GAUtils::printTime(*logFile,"GA execution time:",std::chrono::duration<double,std::milli> (finishGA - startGA).count(),std::chrono::duration<double> (finishGA - startGA).count()); 

    auto finish = std::chrono::high_resolution_clock::now();
    GAUtils::printTime(*logFile,"Total execution time:",std::chrono::duration<double,std::milli> (finish - start).count(),std::chrono::duration<double> (finish - start).count()); 

    GAUtils::printFooter(*logFile,pop,i,firstBestFitness);

    (*logFile).close();
    delete logFile;

}