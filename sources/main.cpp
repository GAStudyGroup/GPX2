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

using std::cout;
using std::endl;
using std::invalid_argument;
using std::srand;
using std::stof;
using std::stoi;
using std::ofstream;

// inicia o algoritmo genético
void GA();

//1 argumento tour_name
//2 caminho para o .tsp
//3 tamanho da pop
//4 ID da run(usado para log
//5 porcentagem de população inicial gerado pelo LK
//6 tipo de geração da nova poop
//7 n best para serem salvos para a proxima geração
//8 best fitness conhecida
// ex: GA berlin52 lib/ 100 3 0.1 1 4 255 
int main(int argc, char *argv[]) {
    srand(time(NULL)); 

    if (argc == 9) {
        try {
            Config::NAME = argv[1];
            Config::LIB_PATH = argv[2];
            Config::POP_SIZE = stoi(argv[3]);
            Config::ID = stoi(argv[4]);
            Config::LK_PERCENTAGE = stof(argv[5]);
            Config::NEW_POP_TYPE = stoi(argv[6]);
            Config::N_BEST = stoi(argv[7]);
            Config::BEST_FITNESS = stoi(argv[8]);
        } catch (invalid_argument &i_a) {
            cout << "Invalid argument!" << i_a.what() << endl;
            return (0);
        }
    } else {
        cout << "Invalid argument number."
             << endl;
        return (0);
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