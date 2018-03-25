#include "GAUtils.hpp"

void GAUtils::init(Population &pop){
    //Import the map
    ImportData dataFile(Config::LIB_PATH+Config::NAME);
    Config::map.setCityList(dataFile.getCitiesCoord());

    //Import the first population from LK
    unsigned lkPop = Config::POP_SIZE * Config::LK_PERCENTAGE;
    unsigned fillPop = Config::POP_SIZE - lkPop;
    if (Config::LK_PERCENTAGE > 0) {
        pop = dataFile.importFirstPopulation(Config::NAME, lkPop);
    }

    //fill the rest of the population with 2-opt
    GAUtils::fillPopulation(pop, fillPop);
}

bool GAUtils::stop(Population &pop) {
    unsigned static generationsWithoutChange{0};
    unsigned static bestFitness{(unsigned)pop.bestFitness()};
    
    unsigned currentFitness{(unsigned)pop.bestFitness()};

    unsigned totalEqual{1};
    //how many routes have fitness equal to the best route, to determine stagnation
    for (Tour t : pop.getPopulation()) {
        if ((unsigned)t.getFitness() == bestFitness) {
            totalEqual++;
        }
    }

    //If the current fitness is equal to or less than the best known, it decreases the number of generations to stop AG
    if(bestFitness <= Config::BEST_FITNESS){
        Config::GENERATION_LIMIT = Config::AFTER_BEST;
    }

    //zero the generationsWithoutChange counter if a better fitness is found
    if (bestFitness > currentFitness) {
        bestFitness = currentFitness;
        generationsWithoutChange = 0;
    } else if (totalEqual == Config::POP_SIZE) {
        //if all elements are equal ends GA
        return (false);
    } else {
        generationsWithoutChange++;
    }

    if (generationsWithoutChange >= Config::GENERATION_LIMIT) {
        return (false);
    } else {
        return (true);
    }
}

void GAUtils::fillPopulation(Population &pop, unsigned popToFill) {
    for (unsigned i = 0; i < popToFill; i++) {
        // Tour t(nearestNeighbor(map));
        Tour t(Config::map.getCityList());
        random_shuffle(t.getRoute().begin(), t.getRoute().end());
        t = Opt::optimize(t);
        pop.getPopulation().push_back(t);
    } 
}


Population GAUtils::generateNewPopulation(Population &pop) {
    if(Config::NEW_POP_TYPE==0){
        return crossAllxAllwith2opt(pop);
    }else if(Config::NEW_POP_TYPE==1){
        return crossNBestxAllwithReset(pop);
    }else{
        return crossAllxAllwithNBestAndReset(pop);
    }
}

Population GAUtils::crossAllxAllwith2opt(Population &pop) {
    Population newPop;
    Tour currentTour;

    for (unsigned i = 0; i < Config::POP_SIZE; i++) {
        currentTour = pop.getPopulation()[i];
        for (unsigned j = 0; j < Config::POP_SIZE; j++) {
            if (i != j) {
                Tour t = GPX2::crossover(pop.getPopulation()[j], currentTour);
                if (t.getFitness() < currentTour.getFitness()) {
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

    sort(pop.getPopulation().begin(), pop.getPopulation().end(),
         [](Tour &a, Tour &b) { return a.getFitness() < b.getFitness(); });

    for (unsigned i = 0; i < Config::N_BEST; i++) {
        Tour savedTour = pop.getPopulation()[i];
        for (Tour t : pop.getPopulation()) {
            Tour tmp = GPX2::crossover(savedTour, t);
            savedTour = tmp;
        }
        newPop.getPopulation().push_back(savedTour);
    }

    fillPopulation(newPop, Config::POP_SIZE - Config::N_BEST);
    return (newPop);
}

Population GAUtils::crossAllxAllwithNBestAndReset(Population &pop){
    Population tmpPop, newPop;
    Tour currentTour;

    for (unsigned i = 0; i < Config::POP_SIZE; i++) {
        currentTour = pop.getPopulation()[i];
        for (unsigned j = 0; j < Config::POP_SIZE; j++) {
            if (i != j) {
                Tour t = GPX2::crossover(pop.getPopulation()[j], currentTour);
                if (t.getFitness() < currentTour.getFitness()) {
                    currentTour = t;
                }
            }
        }
        currentTour = Opt::optimize(currentTour);

        tmpPop.getPopulation().push_back(currentTour);
    }

    sort(tmpPop.getPopulation().begin(), tmpPop.getPopulation().end(),
         [](Tour &a, Tour &b) { return a.getFitness() < b.getFitness(); });

    for(unsigned i=0;i<Config::N_BEST;i++){
        newPop.getPopulation().push_back(tmpPop.getPopulation()[i]);
    }

    fillPopulation(newPop, Config::POP_SIZE - Config::N_BEST);
    return newPop;
}

std::ofstream* GAUtils::initLogFile(){
    string logName{"Logs/"+to_string(Config::NEW_POP_TYPE)+"/log_"+to_string(Config::ID)+"_"+Config::NAME+"_"+to_string(Config::POP_SIZE)+(Config::LK_PERCENTAGE>0?("_LK"):(""))+".log"};

    ofstream *logFile = new ofstream(logName);
    if(!logFile->is_open()){
        std::cout<<"falha na abertura do arquivo de log"<<std::endl;
        exit(0);
    }

    return logFile;
}


void GAUtils::printHeader(std::ostream &out){
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
    }
    out<<endl;
}

void GAUtils::printFooter(std::ostream &out,Population &pop,unsigned gen,unsigned best){
    out << "\nTHE END\n";
    out << "first best fitness: " << best << "\n";
    out << "gen " << gen << " best fitness " << pop.bestFitness() << "\n";
    out << "=========================" << "\n";

    out << "\nLast population\n";
    sort(pop.getPopulation().begin(), pop.getPopulation().end(),
         [](Tour &a, Tour &b) { return a.getFitness() < b.getFitness(); });
    for(Tour t : pop.getPopulation()){
        out<<t<<"\n";
    }
    out<<endl;
}

void GAUtils::printTime(std::ostream &out, std::string txt, double milli, double sec){
    out << "\n"+txt+"\n";
    out << "\t" << milli << " milliseconds\n";
    out << "\t" << sec << " seconds" << endl;
}

/* vector<City> nearestNeighbor() {
    vector<pair<float, City>> cityList;
    for (City c : Config::map.getCityList()) {
        cityList.push_back(make_pair(0, c));
    }
    unsigned choosenCity = rand() % cityList.size();
    vector<City> tour;
    tour.push_back(cityList[choosenCity].second);
    cityList.erase(cityList.begin() + choosenCity);
    while (!cityList.empty()) {
        for (unsigned i = 0; i < cityList.size(); i++) {
            pair<double, double> p1(cityList[i].second.getX(),
                                    cityList[i].second.getY()),
            p2(tour.back().getX(), tour.back().getY());

            cityList[i].first = distance(p1, p2);
        }
        sort(cityList.begin(), cityList.end(), [](auto &left, auto &right) {
            return (right.first > left.first);
        });
        tour.push_back(cityList[0].second);
        cityList.erase(cityList.begin());
    }
    return (tour);
} */