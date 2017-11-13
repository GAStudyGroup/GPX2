#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <fstream>

#include "GPX2.hpp"
#include "ImportData.hpp"
#include "Map.hpp"
#include "Population.hpp"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::random_shuffle;
using std::srand;
using std::stoi;

void GA(string, unsigned);
bool stop(Population);
Population generateNewPopulation(Population, int);
std::ofstream file;
std::ofstream fileFinal;
std::ofstream fileDebugTour;
int id{ 0 };

//primeiro argumento tour_name, segundo tamanho da pop
int main(int argc, char* argv[])
{
    string name{ "" };
    unsigned popSize{ 0 };

    srand(time(NULL));

    if (argc == 4) {
        try {
            name = argv[1];
            popSize = stoi(argv[2]);
            id = stoi(argv[3]);
        } catch (invalid_argument& i_a) {
            cout << "Invalid population size! " << i_a.what() << endl;
            return (0);
        }
    } else if (argc == 0) {
        cout << "Please specify the arguments: tour name and population size." << endl;
        return (0);
    } else {
        cout << "Invalid argument number. Expecting two arguments." << endl;
        return (0);
    }

    GA(name, popSize);

    return 0;
}

void GA(string name, unsigned popSize)
{

    Map map;
    Population pop;

    {
        ImportData dataFile(name);
        //carrega o mapa
        map.setCityList(dataFile.getCitiesCoord());

        //carrega a primeira população
        pop = dataFile.importFirstPopulation(map, name, popSize);
        //pop = dataFile.importFirstPopulation(map,"log",popSize);
    }

    file.open("Logs/logRun_" + to_string(id) + ".log");
    //fileDebug.open("log_"+to_string(id)+"_BUG.txt");

    int i{ 1 }, firstBestFitness{ pop.bestFitness() };
    cout << "First fitness " << firstBestFitness << endl;
    while (stop(pop)) {
        //file<<"BestFitness: "<<pop.bestFitness()<<endl;

        pop = generateNewPopulation(pop, i);

        if (i % 10 == 0) {
            cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
        }
        i++;
    }
    cout << "THE END" << endl;
    cout << "first best fitness: " << firstBestFitness << endl;
    cout << "gen " << i << " best fitness " << pop.bestFitness() << endl;
    cout << "=========================" << endl;

    fileFinal.open(name + "_Run" + to_string(id) +"_FINAL"+ ".opt");

    pop.writeBestTour(fileFinal);

    fileFinal.flush();
    fileFinal.close();


    // DEBUG AREA
    /* cout<<"before"<<endl;
    for(Tour t : pop.getPopulation()){
        cout<<t<<endl;
    }

    pop = generateNewPopulation(pop);

    cout<<"after"<<endl;
    for(Tour t : pop.getPopulation()){
        cout<<t<<endl;
    } */

    /* fileDebug.flush();
    fileDebug.close(); */
    file.flush();
    file.close();
}

bool stop(Population pop)
{

    int static generationsWithoutChange{ 0 };
    int static bestFitness{ pop.bestFitness() };
    int currentFitness{ pop.bestFitness() };

    unsigned totalCon{ 0 };
    for (Tour t : pop.getPopulation()) {
        if (t.getFitness() == bestFitness) {
            totalCon++;
        }
    }

    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
        //cout<<"new best fitness: "<<bestFitness<<endl;
    } else if (totalCon == pop.getPopulation().size()) {
        //cout<<"all the tours are the same"<<endl;
        return (false);
    } else {
        generationsWithoutChange++;
    }
    //cout<<"tours that are the same "<<totalCon<<endl;
    file << "SameTours: " << totalCon << endl;
    if (generationsWithoutChange == 100) {
        return (false);
    } else {
        return (true);
    }
}

Population generateNewPopulation(Population pop, int gen)
{
    unsigned size = pop.getPopulation().size();
    Population newPop;

    //std::random_shuffle(pop.getPopulation().begin(), pop.getPopulation().end());

    for (unsigned i = 0; i < size; i++) {
        if (i == (size - 1)) {

            newPop.addNewTour(GPX2::crossover(pop.getPopulation().at(i), pop.getPopulation().at(0)));

            if (pop.getPopulation().at(i).getFitness() < newPop.getPopulation().back().getFitness() || pop.getPopulation().at(0).getFitness() < newPop.getPopulation().back().getFitness()) {
                fileDebugTour.open("Logs/logCrossover_Run" + to_string(id) + "_Gen" + to_string(i) + ".log");

                fileDebugTour << pop.getPopulation().at(i) << endl;
                fileDebugTour << "\n\n\n\n\n\n" << endl;
                fileDebugTour << pop.getPopulation().at(0) << endl;

                fileDebugTour.flush();
                fileDebugTour.close();
            }

            file << "CROSSOVER " << pop.getPopulation().at(i).getFitness() << " " << pop.getPopulation().at(0).getFitness() << " " << newPop.getPopulation().back().getFitness() << " " << i << " " << 0 << " " << endl;
        } else {

            newPop.addNewTour(GPX2::crossover(pop.getPopulation().at(i), pop.getPopulation().at(i + 1)));

            if (pop.getPopulation().at(i).getFitness() < newPop.getPopulation().back().getFitness() || pop.getPopulation().at(i + 1).getFitness() < newPop.getPopulation().back().getFitness()) {
                fileDebugTour.open("Logs/logCrossover_Run" + to_string(id) + "_Gen" + to_string(i) + ".log");

                fileDebugTour << pop.getPopulation().at(i) << endl;
                fileDebugTour << "\n\n\n\n\n\n" << endl;
                fileDebugTour << pop.getPopulation().at(i + 1) << endl;

                fileDebugTour.flush();
                fileDebugTour.close();
            }

            file << "CROSSOVER " << pop.getPopulation().at(i).getFitness() << " " << pop.getPopulation().at(i + 1).getFitness() << " " << newPop.getPopulation().back().getFitness() << " " << i << " " << i + 1 << " " << endl;
        }
    }

    return newPop;
}