#include "Arg.hpp"

#include <sstream>

Arg::Arg(int argc, char **argv):argc(argc),argv(argv){}

void Arg::newArgument(string arg, bool required, string alias ){
    arguments.push_back(Argument(arg, alias, required));
}

void Arg::validateArguments(){
    processInput();
    for(Argument a : arguments){
        if(a.getRequired() && a.getOption().empty()){
            throw std::runtime_error("Required argument: "+a.getArg());
        }
    }
}

void Arg::processInput(){
    string argString{""};
    vector<string> argVector;
    for(int i=1;i<argc;i++){
        argString += argv[i];
        argString += " ";
    }
    argVector = split(argString,'-');
    for(string arg : argVector){
        vector<string> argSplited = split(arg);
        Argument *a = getArgument(argSplited[0]);
        if(a==NULL){
            throw std::runtime_error("Unexpected argument: "+argSplited[0]);
        }
        string option;
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

Arg::Argument* Arg::getArgument(string arg){
    for(Argument &a : arguments){
        if(a.getArg() == arg || a.getAlias() == arg){
            return(&a);
        } 
    }
    return(NULL);
}

bool Arg::isSet(string arg){
    Argument *a = getArgument(arg);
    if(a!=NULL){
        return(a->isSet());
    }
    return false;
}

string Arg::getOption(string arg){
    Argument *a = getArgument(arg);
    if(a!=NULL){
        return(a->getOption());
    }
    return {};
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