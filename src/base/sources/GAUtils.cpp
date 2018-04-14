#include "GAUtils.hpp"

#include <algorithm>
using std::sort;
using std::shuffle;
using std::for_each;
using std::remove;

#include <random>
using std::uniform_real_distribution;
using std::uniform_int_distribution;

#include "Globals.hpp"
#include "Opt.hpp"
#include "GPX2.hpp"
#include "MOC.hpp"
#include "HCO.hpp"
#include "ImportData.hpp"

void GAUtils::init(Population &pop){
    //Import the map
    ImportData dataFile(Config::LIB_PATH+Config::NAME);
    Globals::map.setCityList(dataFile.getCitiesCoord());

    //Import the first population from LK
    unsigned lkPop = Config::POP_SIZE * Config::LK_PERCENTAGE; 
    unsigned fillPop = Config::POP_SIZE - lkPop;
    if (Config::LK_PERCENTAGE > 0) { 
        pop = dataFile.importFirstPopulation(Config::NAME, lkPop);
    }

    //fill the rest of the population with 2-opt
    GAUtils::fillPopulationWithReset(pop, fillPop);
}

bool GAUtils::stop(Population &pop, ostream &out) {
    unsigned static generationsWithoutChange{0};
    unsigned static bestFitness{(unsigned)pop.bestFitness()};
    
    unsigned currentFitness{(unsigned)pop.bestFitness()};

    unsigned totalEqual{0};
    //how many routes have fitness equal to the best route, to determine stagnation
    for (vector<int> t : pop.getPopulation()) {
        if ((unsigned)getFitness(t) == bestFitness) {
            totalEqual++;
        }
    }

    //zero the generationsWithoutChange counter if a better fitness is found
    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
    } else if (totalEqual == Config::POP_SIZE) {
        //if all elements are equal ends GA
        out << "\nPopulation converged!";
        out.flush();
        return (false);
    } else {
        generationsWithoutChange++;
    }

    //If the current fitness is equal to or less than the best known, it decreases the number of generations to stop AG
    if(bestFitness <= Config::BEST_FITNESS){
        // Config::GENERATION_LIMIT = Config::AFTER_BEST;
        out << "\nFound best fitness!\n";
        out.flush();
        return (false);
    }

    if (generationsWithoutChange >= Config::GENERATION_LIMIT) {
        out << "\nGeneration limit reached!";
        out.flush();
        return (false);
    } else {
        return (true);
    }
}

void GAUtils::fillPopulationWithReset(Population &pop, unsigned popToFill) {
    for (unsigned i = 0; i < popToFill; i++) {
        vector<int> t;
        //random
        if(Config::NEW_TOUR_MODE==0 || Config::NEW_TOUR_MODE==1){
            t = Globals::map.getCityList();
            shuffle(t.begin(), t.end(),Globals::urng);
        }else{
            t = nearestNeighbor();
        }
        //apply 2-opt
        if(Config::NEW_TOUR_MODE==1 || Config::NEW_TOUR_MODE==2){
            t = Opt::optimize(t); 
        }
        
        pop.getPopulation().push_back(t);
    } 
}

void GAUtils::fillPopulationWithRoulete(Population &pop,Population &newPop, unsigned popToFill) {
    for(unsigned i=0;i<popToFill;i++){
        newPop.getPopulation().push_back(roulete(pop));
    }
}

Population GAUtils::generateNewPopulation(Population &pop) {
    if(Config::NEW_POP_TYPE==0){
        return crossAllxAllwith2opt(pop);
    }else if(Config::NEW_POP_TYPE==1){
        return crossNBestxAllwithReset(pop);
    }else if(Config::NEW_POP_TYPE==2){
        return crossAllxAllwithNBestAndReset(pop);
    }else if(Config::NEW_POP_TYPE==3){
        return crossWithMOC(pop);
    }else{
        //default
        return crossNBestxAllwithReset(pop);
    }
}

Population GAUtils::crossAllxAllwith2opt(Population &pop) {
    Population newPop;
    vector<int> currentTour;

    for (unsigned i = 0; i < Config::POP_SIZE; i++) {
        currentTour = pop.getPopulation()[i];
        for (unsigned j = 0; j < Config::POP_SIZE; j++) {
            if (i != j) {
                vector<int> t = GPX2::crossover(pop.getPopulation()[j], currentTour);
                if (getFitness(t) < getFitness(currentTour)) {
                    currentTour = t;
                }
            }
        }
        currentTour = Opt::optimize(currentTour);

        newPop.getPopulation().push_back(currentTour);
    }

    return newPop;
}

Population GAUtils::crossNBestxAllwithReset(Population &pop) {
    Population newPop;

    sort(pop.getPopulation().begin(), pop.getPopulation().end(),sortPopulation);

    for (unsigned i = 0; i < Config::N_BEST; i++) {
        vector<int> savedTour = pop.getPopulation()[i];
        for (vector<int> t : pop.getPopulation()) {
            vector<int> tmp = GPX2::crossover(savedTour, t);
            savedTour = tmp;
        }
        newPop.getPopulation().push_back(savedTour);
    }

    fillPopulationWithReset(newPop, Config::POP_SIZE*Config::RESET_PERCENTAGE);

    fillPopulationWithRoulete(pop,newPop,Config::POP_SIZE-newPop.getPopulation().size());
    
    return (newPop);
}

Population GAUtils::crossAllxAllwithNBestAndReset(Population &pop){
    Population tmpPop, newPop;
    vector<int> currentTour;

    for (unsigned i = 0; i < Config::POP_SIZE; i++) {
        currentTour = pop.getPopulation()[i];
        for (unsigned j = 0; j < Config::POP_SIZE; j++) {
            if (i != j) {
                vector<int> t = GPX2::crossover(pop.getPopulation()[j], currentTour);
                currentTour = t;
            }
        }

        tmpPop.getPopulation().push_back(currentTour);
    }

    sort(tmpPop.getPopulation().begin(), tmpPop.getPopulation().end(),sortPopulation);

    for(unsigned i=0;i<Config::N_BEST;i++){
        newPop.getPopulation().push_back(tmpPop.getPopulation()[i]);
    }

    fillPopulationWithReset(newPop, Config::POP_SIZE*Config::RESET_PERCENTAGE);

    fillPopulationWithRoulete(pop,newPop,Config::POP_SIZE-newPop.getPopulation().size());

    return newPop;
}

Population GAUtils::crossWithMOC(Population &pop){
    Population newPop;
    vector<vector<int>> bestTours{elitsm(pop)};
    newPop.getPopulation().insert(newPop.getPopulation().end(),bestTours.begin(),bestTours.end());

    // for(unsigned i=0;i<(0.5*Config::POP_SIZE);i++)
    for(unsigned i=0;i<(Config::POP_SIZE - newPop.getPopulation().size());i++)
    {
        newPop.getPopulation().push_back(HCO::cross(roulete(pop),roulete(pop)));
    }

    /* fillPopulation(newPop,(Config::POP_SIZE-newPop.getPopulation().size())); */

    return(newPop);
}

vector<vector<int>> GAUtils::elitsm(Population pop){
    sort(pop.getPopulation().begin(),pop.getPopulation().end(),sortPopulation);
    return(vector<vector<int>>(pop.getPopulation().begin(),pop.getPopulation().begin()+Config::N_BEST));
}

vector<int> GAUtils::roulete(Population &pop){
    double sumFitness{0.0},ctrl{0.0};

    for_each(pop.getPopulation().begin(),pop.getPopulation().end(),[&sumFitness](vector<int> t){sumFitness+=(1.0/(double)getFitness(t));});    

    uniform_real_distribution<double> dist(0,sumFitness);
    ctrl = dist(Globals::urng);

    int i=0;
    for(vector<int> t : pop.getPopulation()){
        ctrl -= (1.0/(double)getFitness(t));
        if(ctrl<=0){
            return(t);
        }
        i++;
    }
    return(pop.getPopulation().back());
}



vector<int> GAUtils::nearestNeighbor() {
    vector<int> cityList{Globals::map.getCityList()},tmp;
    int delta = cityList.size()*0.05;
    delta = delta<1?1:delta;

    uniform_int_distribution<int> dist(0,cityList.size()-1);
    uniform_int_distribution<int> distDelta(0,delta);

    int choosenCity = cityList[dist(Globals::urng)];
    tmp.push_back(choosenCity);
    cityList.erase(remove(cityList.begin(),cityList.end(),choosenCity),cityList.end());

    do{

        sort(cityList.begin(),cityList.end(),[choosenCity](int a,int b){
            return distance(a,choosenCity) < distance(b,choosenCity);
        });

        unsigned next = distDelta(Globals::urng);

        if(next >= cityList.size()){
            next = cityList.size()-1;
        }

        choosenCity = cityList[next];

        tmp.push_back(choosenCity);
        cityList.erase(remove(cityList.begin(),cityList.end(),choosenCity),cityList.end());

    }while(!cityList.empty());

    return tmp;
}