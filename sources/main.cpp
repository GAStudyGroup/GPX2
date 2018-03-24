#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "Config.hpp"
#include "Population.hpp"
#include "Utils.hpp"
#include "GAUtils.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::random_shuffle;
using std::ref;
using std::sort;
using std::srand;
using std::stof;
using std::stoi;
using std::ofstream;

// inicia o algoritmo genético
void GA();

//1 argumento tour_name, 
//2 caminho para o .tsp, 
//3 tamanho da pop, 4 ID da run(usado para log
//5 porcentagem de população inicial gerado pelo LK
//6 tipo de geração da nova poop
//7 best fitness conhecida
// ex: GA berlin52 lib/ 100 0.1 0
int main(int argc, char *argv[]) {

    srand(time(NULL)); 

    if (argc == 8) {
        try {
            Config::NAME = argv[1];
            Config::LIB_PATH = argv[2];
            Config::POP_SIZE = stoi(argv[3]);
            Config::ID = stoi(argv[4]);
            Config::LK_PERCENTAGE = stof(argv[5]);
            Config::NEW_POP_TYPE = stoi(argv[6]);
            Config::BEST_FITNESS = stoi(argv[7]);
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

    GAUtils::init(pop);

    GAUtils::printHeader(*logFile);

    int i{1}, firstBestFitness{pop.bestFitness()};
    *logFile << "First fitness " << firstBestFitness << endl;
    do {
        pop = GAUtils::generateNewPopulation(pop);
        *logFile << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        i++;
    } while (GAUtils::stop(pop));

    GAUtils::printFooter(*logFile,pop,i,firstBestFitness);

    (*logFile).close();
    delete logFile;
}