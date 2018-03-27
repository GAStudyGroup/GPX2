#ifndef ARG_HPP
#define ARG_HPP

#include <vector>
#include <string>


using std::string;
using std::vector;
using std::istringstream;

class Arg{
    public:
        Arg(int ,char **);

        bool isSet(string);

        string getOption(string);
    private:
        vector<string> split(const string , char = ' ');

        using argument = struct argument{
            string arg;
            string option;
            argument(string arg, string option):arg(arg),option(option){}
        };

        vector<argument> arguments;
};

#endif