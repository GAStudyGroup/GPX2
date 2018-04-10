#include "ImportData.hpp"
#include "Globals.hpp"

#include <regex>
using std::regex;

#include <iostream>
using std::cout;
using std::endl;


#include <sstream>
using std::ostringstream;

//from string
using std::to_string;
using std::stod;
using std::stoi;

ImportData::ImportData(string nome)
{
    string input;
    myfile.open(nome + ".tsp");
    if (!myfile.is_open()) {
        cout << "Falha na leitura do arquivo" << endl;
        exit(EXIT_FAILURE);
    }
    myfile >> input;
    regexManager(input);
    while (myfile >> input) {
        regexManager(input);
        if (findIgnoredWords(input)) {
            reader(flag, input);
        }
    }
}

void ImportData::regexManager(string input)
{
    //|[[:digit:]]+?=[\\.]?=[[:digit:]]+
    regex number("[[:digit:]]+");
    regex name("NAME(.*)");
    regex type("TYPE(.*)");
    regex comment("COMMENT(.*)");
    regex dimension("DIMENSION(.*)");
    regex edge_type("EDGE(.*)");
    regex node_comment("NODE(.*)");
    if (regex_match(input, name)) {
        flag = "name";
        //cout << "Nome\n";
    } else if (regex_match(input, type)) {
        flag = "type";
        //cout << "Tipo\n";
    } else if (regex_match(input, comment)) {
        flag = "comment";
    } else if (regex_match(input, dimension)) {
        flag = "dimension";
    } else if (regex_match(input, edge_type)) {
        flag = "edge_type";
    } else if (regex_match(input, node_comment)) {
        flag = "node_comment";
    } else if (regex_match(input, number) && flagaux == "wait for number") {
        flag = "number";
    }
}

bool ImportData::findIgnoredWords(string input)
{
    regex ignore(":|NAME(.*)|TYPE(.*)|COMMENT(.*)|DIMENSION(.*)|EDGE(.*)|EOF");
    if (regex_match(input, ignore)) {
        return false;
    }
    //  cout << input;
    return true;
}

void ImportData::reader(string flag, string input)
{
    if (flag == "name") {
        tspName = input;
        //cout << input << " is a name! \n";
    } else if (flag == "type") {
        type = input;
        //cout << input << " is a type! \n";
    } else if (flag == "comment") {
        comment += (input + " ");
        //cout << input << " is a comment! \n";
    } else if (flag == "dimension") {
        //cout << input << " is a dimension! \n";
        citiescoord.reserve(stoi(input));
        flagaux = "wait for number";
    } else if (flag == "edge_type") {
        edge_type = input;
        setEdge_Type(input);
        //cout << input << " is an edge_type! \n";
    } else if (flag == "node_comment") {
        node_comment = input;
        //cout << input << " is an node_comment! \n";
    } else if (flag == "number" && flagaux == "wait for number") {
        int id = stod(input);
        //cout << "I: " << input;
        myfile >> input;
        double x = stod(input);
        //cout << "\t X: " << input;
        myfile >> input;
        double y = stod(input);
        //cout << "\t Y: " << input << "\n";
        citiescoord.push_back(City(id, x, y));
    }
}
void ImportData::printInfos()
{
    cout << "Nome: " << gettspName() << endl;
    cout << "Tipo: " << getType() << endl;
    cout << "Comentário: " << getcomment() << endl;
    cout << "Tipo do Vértice: " << getedge_type() << endl;
    cout << "Comentário do tipo de node: " << getnode_comment() << endl;
}
string ImportData::getInfos()
{
    string input;
    cout << "BATATA";
    input = "Nome: " + gettspName() + "\n";
    input = input + "Tipo: " + getType() + "\n";
    input = input + "Comentário: " + getcomment() + "\n";
    input = input + "Tipo do Vértice: " + getedge_type() + "\n";
    input = input + "Comentário do tipo de node: " + getnode_comment() + "\n";
    cout << input;
    return input;
}

vector<City> ImportData::getCitiesCoord()
{
    return citiescoord;
}
string ImportData::gettspName()
{
    return tspName;
}
string ImportData::getType()
{
    return type;
}
string ImportData::getedge_type()
{
    return edge_type;
}
string ImportData::getnode_comment()
{
    return node_comment;
}
string ImportData::getcomment()
{
    if (comment == "") {
        return "N/A";
    } else {
        return comment;
    }
}

void ImportData::setEdge_Type(string type){
    if(!type.compare("EUC_2D")){
        Config::TYPE = Config::type::EUC_2D;
    }else if(!type.compare("GEO")){
        Config::TYPE = Config::type::GEO;
    }else if(!type.compare("ATT")){
        Config::TYPE = Config::type::ATT;
    }else{
        cout<<"Unknown edge type."<<endl;
        exit(0);
    }

}

Population ImportData::importFirstPopulation(string name, unsigned popSize)
{
    ifstream file;
    string word{ "" };
    Population pop;
    for (unsigned i = 1; i <= popSize; i++) {
        string fileName{ name + "_exec_1_sol_" + to_string(i) + "_" + to_string(popSize) + ".dat" };
        /* string fileName{ name + to_string(i) + ".dat2" }; */
        file.open(fileName);
        if (!file.is_open()) {
            cout << "error reading file" << endl;
            exit(EXIT_FAILURE);
        }

        vector<int> newT;
        newT.reserve(popSize);
        auto ss = ostringstream{};
        ss <<file.rdbuf();
        auto cities = explode(ss.str(),'\n');
        //while (file >> word) {
        for(auto wordaux : cities){
            newT.push_back(stoi(wordaux));
        }
        //}
        file.close();

        pop.getPopulation().push_back(newT);
    }
    return (pop);
}

std::vector<std::string> ImportData::explode(std::string const & s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(std::move(token));
    }

    return result;
}


/* Population ImportData::importFirstPopulation(Map map,string name,unsigned popSize){
    ifstream file;
    string word{""};
    Population pop;
    for(unsigned i=1;i<3;i++){
        //string fileName{name+"_exec_1_sol_"+to_string(i)+"_"+to_string(popSize)+".dat"};
        cout << name+"_"+to_string(i)+".log"<<endl;
        string fileName{name+"_"+to_string(i)+".log"};
        file.open(fileName);
        if(!file.is_open()){
            cout<<"error reading file"<<endl;
            exit(EXIT_FAILURE);
        }

        Tour newT;
        //newT.getRoute().reserve(popSize);
        while(file>>word){
            newT.getRoute().push_back(map.getCityById(stoi(word)));
        }
        file.close();

        pop.addNewTour(newT);
    }
    return(pop);
} */