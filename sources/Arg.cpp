#include "Arg.hpp"

#include <sstream>

Arg::Arg(int argc, char **argv) {
    string tmp{""};
    vector<string> tmp2;
    for (unsigned i = 1; i < argc; i++) {
        tmp += argv[i];
        tmp += " ";
    }
    tmp2 = split(tmp, '-');

    for (string s : tmp2) {
        vector<string> args = split(s);
        if (args.size() == 1) {
            args.push_back("");
        }
        arguments.push_back(argument(args[0], args[1]));
    }
}

bool Arg::isSet(string arg) {
    for (argument a : arguments) {
        if (a.arg == arg) {
            return true;
        }
    }
    return false;
}

string Arg::getOption(string arg) { 
    for (argument a : arguments) {
        if (a.arg == arg) {
            return a.option;
        }
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