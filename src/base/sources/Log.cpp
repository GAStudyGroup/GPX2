#include "Log.hpp"

#include "Globals.hpp"

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::to_string;

#include <algorithm>
using std::sort;

ofstream* Log::initLogFile(){
    string logName{"log/"+to_string(Config::NEW_POP_TYPE)+"/log_"+to_string(Config::ID)+"_"+Config::NAME+"_"+to_string(Config::POP_SIZE)+(Config::LK_PERCENTAGE>0?("_LK-"+to_string(Config::LK_PERCENTAGE*100)):(""))+"_"+to_string(Config::NEW_TOUR_MODE)+".log"};
    
    ofstream *logFile = new ofstream(logName);
    if(!logFile->is_open()){
        cout<<"falha na abertura do arquivo de log"<<endl;
        exit(0);
    }

    return logFile;
}


void Log::printHeader(ostream &out){
    out << "Run id: "+to_string(Config::ID)+"\nGenetic Algoritm for problem "+Config::NAME+" with population size "+to_string(Config::POP_SIZE);
    out << (Config::LK_PERCENTAGE>0?(", using "+to_string(Config::LK_PERCENTAGE*100)+"% of LK generated tours"):(""));
    out <<"\nUsing new generation method: ";
    switch(Config::NEW_POP_TYPE){
        case 0:{
            out << "Cross using OX";
            break;
        }
        default:
        case 1:{
            out << " All vs All GPX2 crossover saving "+to_string(Config::N_BEST)+" with reset population and roulete.";
            break;
        }
        case 2:{
            out << "GPX2 with Ant Colony to introduce randomness";
            break;
        }
    }
    out<<"\nBest known solution: "<<to_string(Config::BEST_FITNESS)<<".\n";
    out.flush();
}

void Log::printFooter(ostream &out,Population &pop,unsigned gen,unsigned best){
    out << "\nTHE END\n";
    out << "First best fitness: " << best << "\n";
    out << "Last gen: " << gen << "\n";
    out << "Best fitness found: " << pop.bestFitness() << "\n";
    out << "Best know solution: "<< to_string(Config::BEST_FITNESS) << "\n";
    out << "=========================" << "\n";

    sort(pop.getPopulation().begin(),pop.getPopulation().end(),[](vector<int> &a, vector<int> &b) { return getFitness(a) < getFitness(b);});

    out << "\nLast population\n";
    for(vector<int> t : pop.getPopulation()){
        printTour(t,out);
        out<<"\n";
    }
    out.flush();
}

void Log::printTime(ostream &out, string txt, double milli){
    out << "\n"+txt+"\n";
    out << "\t" << milli << " milliseconds.\n";
    out << "\t" << (milli/=1000.0) << " seconds.\n";
    out << "\t" << (milli/=60.0) << " minutes.\n";
    out.flush();
}

void Log::printTourFile(vector<int> &tour,string name){
    string fileName{name+".txt"};
    ofstream file(fileName);

    if(file.is_open()){
        printTour(tour,file);
    }
}