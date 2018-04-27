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
    string logName{"log/"+to_string(Config::NEW_POP_TYPE)+"/log_"+to_string(Config::ID)+"_"+Config::NAME+"_"+to_string(Config::POP_SIZE)+(Config::LK_PERCENTAGE>0?("_LK"):(""))+".log"};

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

void Log::printFooter(ostream &out,Population &pop,unsigned gen,unsigned best){
    out << "\nTHE END\n";
    out << "First best fitness: " << best << "\n";
    out << "Gen " << gen << " best fitness " << pop.bestFitness() << "\n";
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

void Log::printTourFile(vector<int> &tour,string name,string color){
    string fileName{name+".txt"};
    ofstream file(fileName);

    if(file.is_open()){
        file<<"//#color "<<color<<"\n";
        file<<"//#name "<<name<<"\n";
        file<<"//#fitness "<<getFitness(tour)<<"\n";
        
        printTour(tour,file);
    }

    file.flush();
    file.close();
}