#include "Arg.hpp"

#include <sstream>
using std::istringstream;

#include <set>
using std::set;

#include <algorithm>
using std::any_of;

#include <iostream>
using std::cout;
using std::endl;

Arg::Arg(int argc, char **argv):argc(argc),argv(argv){
    programName = "Default Name";
    helpSetted = false;
}

void Arg::newArgument(string arg, bool required, string description){
    vector<string> argV{split(arg,'|')};
    arguments.push_back(Argument(argV, required, description));
}

void Arg::validateArguments(){
    //programmer error, name or alias repeated
    nameConflict();
    
    processInput();
    if(helpSetted && isSet("help")){
        throw std::runtime_error(help());
    }else{
        processRequired();
    }
}

void Arg::processRequired(){
    bool error{false};
    string errorMsg{""};
    for(Argument a : arguments){
        if(a.getRequired() && a.getOption().empty()){
            if(error){
                errorMsg += "\n";
            }
            error = true;
            errorMsg += "Required argument: "+a.getArg()[0];
        }
    }
    throwException(errorMsg);
}

void Arg::processInput(){
    vector<string> argVector;
    bool error{false};
    string errorMsg{""};

    for(int i=1;i<argc;i++){
        string arg = argv[i];
        if(arg[0] == '-'){
            arg.erase(0,1);
            argVector.push_back(arg);
        }else{
            argVector.back() += " ";
            argVector.back() += arg;
        }
    }
    
    for(string arg : argVector){
        vector<string> argSplited = split(arg);
        Argument *a = getArgument(argSplited[0]);
        if(a==nullptr){
            if(error){
                errorMsg += "\n";
            }
            error = true;
            errorMsg += "Unexpected argument: "+argSplited[0];
        }else{
            string option{""};
            for(unsigned i=1;i<argSplited.size();i++){
                option += argSplited[i];
                if((++i)<argSplited.size()){
                    option += " ";
                }
            }
            a->setOption(option);
            a->setArg(true);
        }
    }
    throwException(errorMsg);
}

void Arg::throwException(string errorMsg){
    if(!errorMsg.empty()){
        if(helpSetted){
            errorMsg += "\nUse -help or -h to display program options.";
        }
        throw std::runtime_error(errorMsg);
    }
}

string Arg::help(){
    string tmp{};

    tmp += programName+"\n\n";
    tmp += "Options:\n";

    for(Argument a : arguments){
        tmp += a.to_string();
        tmp += "\n\n";
    }

    return(tmp);
}

Arg::Argument* Arg::getArgument(string arg){
    vector<string> argV = split(arg,'|');
    for(string s : argV){
        for(Argument &a : arguments){
            for(string name : a.getArg()){
                if(name == s){
                    return(&a);
                } 
            }
        }
    }
    
    return(nullptr);
}

bool Arg::isSet(string arg){
    Argument *a = getArgument(arg);
    if(a!=nullptr){
        return(a->isSet());
    }
    return false;
}

string Arg::getOption(string arg){
    Argument *a = getArgument(arg);
    if(a!=nullptr){
        return(a->getOption());
    }
    return {};
}

void Arg::nameConflict(){
    set<string> conflicting;
    for(unsigned i=0;i<arguments.size();i++){
        for(string n : arguments[i].getArg()){
            for(unsigned j=0;j<arguments.size();j++){
                if(i!=j){
                    vector<string> tmp{arguments[j].getArg()};
                    if(find(tmp.begin(),tmp.end(),n)!=tmp.end()){
                        conflicting.insert(n);
                    }
                }
            }
        }
    }

    bool error{false};
    string errorMsg{};
    for(string n : conflicting){
        if(error){
            errorMsg+="\n";
        }
        error = true;
        errorMsg += "Conflicting name or alias: "+n;
    }            
    throwException(errorMsg);
}

void Arg::setProgramName(string name){
    this->programName = name;
}

void Arg::setHelp(){
    helpSetted = true;
    newArgument("help|h",false,"Display options available.");
}

vector<string> Arg::split(const string s, char c) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, c)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}