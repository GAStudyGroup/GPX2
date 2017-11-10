#include <iostream>

#include <fstream>

#include "Map.hpp"
#include "Population.hpp"
#include "ImportData.hpp"

using std::cout;
using std::endl;
using std::stoi;
using std::to_string;
using std::invalid_argument;

using std:: ifstream;

//primeiro argumento tour_name, segundo tamanho da pop
int main(int argc, char *argv[]){
    string name{""};
    int popSize{0};

    if(argc == 3){
        try{
            name = argv[1];
            popSize = stoi(argv[2]);
        }catch(invalid_argument &i_a){
            cout<<"Invalid population size! "<<i_a.what()<<endl;
            return(0);
        }
    }else if(argc == 0){
        cout<<"Please specify the arguments: tour name and population size."<<endl;
        return(0);
    }else{
        cout<<"Invalid argument number. Expecting two arguments."<<endl;
        return(0);
    }

    GA(name,popSize);

    return 0;
}

void GA(string name,int popSize){

    Map map;

    {
        ImportData dataFile(name);
        map.setCityList(dataFile.getCitiesCoord());
    }

}

void readLKPop(Population pop,string name,int popSize){
    ifstream file;
    for(unsigned i=1;i<=popSize;i++){
        file.open(name+"_exec_1_"+to_string(i)+"_"+to_string(popSize)+".dat");
    }
    
}