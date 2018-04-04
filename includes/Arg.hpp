#ifndef ARG_HPP
#define ARG_HPP

#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

#include <iostream>
using std::cout;
using std::endl;

using std::string;
using std::vector;
using std::istringstream;
using std::pair;
using std::make_pair;

class Arg{
    private:
        class Argument{
            public:
                Argument(){}
                Argument(string arg, string alias, bool required):alias(alias),required(required){
                    setArg(arg);
                    option = "";
                }

                string getArg(){return arg;}
                string getOption(){return option;}
                string getAlias(){return alias;}
                bool getRequired(){return required;}
                bool isSet(){return set;}

                void setOption(string option){
                    this->option = option;
                }

                void setArg(bool set){
                    this->set = set;
                }

            private:
                string arg, alias;
                bool required, set{false};
                string option;

                void setArg(string arg){
                    if(arg.empty()){
                        throw std::invalid_argument("empty string");
                    }else{
                        this->arg = arg;
                    }
                }

                
        };


        vector<string> split(const string , char = ' ');

        vector<Argument> arguments;
        int argc;
        char **argv;

        void processInput();

        Argument* getArgument(string);

    public:
        Arg(int ,char **);

        void newArgument(string, bool = false, string = "");

        void validateArguments();

        bool isSet(string);

        string getOption(string);
};

#endif