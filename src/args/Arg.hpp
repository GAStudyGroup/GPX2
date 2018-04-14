#ifndef ARG_HPP
#define ARG_HPP

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <stdexcept>
//invalid_argument and runtime_exception

class Arg{
    private:
        class Argument{
            public:
                Argument(){}
                Argument(vector<string> arg, bool required, string description):description(description),required(required){
                    setArg(arg);
                    set = false;
                    option = "";
                }

                vector<string> getArg(){return arg;}
                string getOption(){return option;}
                bool getRequired(){return required;}
                bool isSet(){return set;}

                void setOption(string option){
                    this->option = option;
                }

                void setArg(bool set){
                    this->set = set;
                }

                string to_string(){
                    string tmp{};
                    for(unsigned i=0;i<arg.size();i++){
                        tmp += arg[i];
                        if(i+1 != arg.size()){
                            tmp += "|";
                        }
                    }
                    tmp += ((required)?("\n\tRequired."):(""));
                    tmp += "\n\tDescription: "+((description.empty())?("no description."):(description));
                    return(tmp);
                }

            private:
                vector<string> arg;
                string description;
                bool required, set;
                string option;

                void setArg(vector<string> arg){
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
        string programName;
        bool helpSetted;

        void processInput();
        void processRequired();
        void throwException(string);

        Argument* getArgument(string);

        void nameConflict();

        string help();

    public:
        Arg(int ,char **);

        void setProgramName(string name);

        void newArgument(string, bool = false, string = "");

        void validateArguments();

        void setHelp();

        bool isSet(string);

        string getOption(string);
};

#endif