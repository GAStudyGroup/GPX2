#include "GAUtils.hpp"

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::to_string;

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
    GAUtils::fillPopulation(pop, fillPop);
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

void GAUtils::fillPopulation(Population &pop, unsigned popToFill) {
    for (unsigned i = 0; i < popToFill; i++) {
        // vector<int> t(Globals::map.getCityList());
        // shuffle(t.begin(), t.end(),Globals::urng);
        vector<int> t{nearestNeighbor()};
        t = Opt::optimize(t); 
        pop.getPopulation().push_back(t);
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
        exit(1);
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
    unsigned newPopSize;

    sort(pop.getPopulation().begin(), pop.getPopulation().end(),sortPopulation);

    for (unsigned i = 0; i < Config::N_BEST; i++) {
        vector<int> savedTour = pop.getPopulation()[i];
        for (vector<int> t : pop.getPopulation()) {
            vector<int> tmp = GPX2::crossover(savedTour, t);
            savedTour = tmp;
        }
        newPop.getPopulation().push_back(savedTour);
    }

    fillPopulation(newPop, Config::POP_SIZE*Config::RESET_PERCENTAGE);

    newPopSize = newPop.getPopulation().size();

    for(unsigned i=0;i<Config::POP_SIZE-newPopSize;i++){
        newPop.getPopulation().push_back(roulete(pop));
    }
    
    return (newPop);
}

Population GAUtils::crossAllxAllwithNBestAndReset(Population &pop){
    Population tmpPop, newPop;
    vector<int> currentTour;
    unsigned newPopSize;

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

    fillPopulation(newPop, Config::POP_SIZE*Config::RESET_PERCENTAGE);

    newPopSize = newPop.getPopulation().size();

    for(unsigned i=0;i<Config::POP_SIZE-newPopSize;i++){
        newPop.getPopulation().push_back(roulete(pop));
    }

    return newPop;
}

Population GAUtils::crossWithMOC(Population &pop){
    Population newPop;
    vector<vector<int>> bestTours{elitsm(pop)};
    newPop.getPopulation().insert(newPop.getPopulation().end(),bestTours.begin(),bestTours.end());

    // for(unsigned i=0;i<(0.5*Config::POP_SIZE);i++)
    for(unsigned i=0;i<(Config::POP_SIZE - newPop.getPopulation().size());i++){
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

ofstream* GAUtils::initLogFile(){
    string logName{"log/"+to_string(Config::NEW_POP_TYPE)+"/log_"+to_string(Config::ID)+"_"+Config::NAME+"_"+to_string(Config::POP_SIZE)+(Config::LK_PERCENTAGE>0?("_LK"):(""))+"_RANDOM.log"};

    ofstream *logFile = new ofstream(logName);
    if(!logFile->is_open()){
        cout<<"falha na abertura do arquivo de log"<<endl;
        exit(0);
    }

    return logFile;
}


void GAUtils::printHeader(ostream &out){
    out << "Run id: "+to_string(Config::ID)+"\nGenetic Algoritm for problem "+Config::NAME+" with population size "+to_string(Config::POP_SIZE);
    out << (Config::LK_PERCENTAGE>0?(", using "+to_string(Config::LK_PERCENTAGE*100)+"% of LK generated tours"):(""));
    out <<"\nUsing new generation method: ";
    switch(Config::NEW_POP_TYPE){
        case 0:{
            out << "All vs All GPX2 crossover.";
            break;
        }
        case 1:{
            out << to_string(Config::N_BEST)+" best vs All GPX2 crossover with reset population.";
            break;
        }
        case 2:{
            out << "All vs ALL with "+to_string(Config::N_BEST)+" best saved to the next population and reset in the rest.";
            break;
        }
        case 3:{
            out<<"Using elitsm and Modified Order Crossover";
            break;
        }
    }
    out<<"\nBest known solution: "<<to_string(Config::BEST_FITNESS)<<".\n";
    out.flush();
}

void GAUtils::printFooter(ostream &out,Population &pop,unsigned gen,unsigned best){
    out << "\nTHE END\n";
    out << "First best fitness: " << best << "\n";
    out << "Gen " << gen << " best fitness " << pop.bestFitness() << "\n";
    out << "Best know solution: "<< to_string(Config::BEST_FITNESS) << "\n";
    out << "=========================" << "\n";

    out << "\nLast population\n";
    sort(pop.getPopulation().begin(), pop.getPopulation().end(),sortPopulation);
    for(vector<int> t : pop.getPopulation()){
        printTour(t,out);
        out<<"\n";
    }
    out.flush();
}

void GAUtils::printTime(ostream &out, string txt, double milli, double sec){
    out << "\n"+txt+"\n";
    out << "\t" << milli << " milliseconds.\n";
    out << "\t" << sec << " seconds.\n";
    out << "\t" << (sec/60.0) << " minutes.\n";
    out.flush();
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

        int next = distDelta(Globals::urng);

        if(next >= cityList.size()){
            next = cityList.size()-1;
        }

        choosenCity = cityList[next];

        tmp.push_back(choosenCity);
        cityList.erase(remove(cityList.begin(),cityList.end(),choosenCity),cityList.end());

    }while(!cityList.empty());

    return tmp;
}