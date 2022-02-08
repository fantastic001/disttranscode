
#include <cmd/ArgumentParser.hpp>
#include <data/FilterManager.hpp>
#include <algorithm>

using namespace dtcode::cmd;
using namespace dtcode::data;
using namespace std;

ArgumentParser::ArgumentParser(int argc, char** argv)
{
    this->argc = argc;
    this->argv = argv;
}

vector<FilterPtr> ArgumentParser::getFilters() {
    auto arguments = parse(argc, argv);
    vector<FilterPtr> result; 
    auto filter_flags = {"--filter", "-f"};
    for (int i = 0; i<arguments.size(); i++) {
        if (arguments[i] == "--filter" || arguments[i] == "-f") {
            if (i == arguments.size() - 1) return result; 
            else {
                auto parser = FilterManager::getInstance()->findFilter(arguments[i]);
                auto factory = parser->parse(
                    arguments.begin() + i + 2, 
                    find_first_of(arguments.begin()+i+2, arguments.end(), filter_flags.begin(), filter_flags.end())
                );
                result.push_back(factory->create());
            }
        }
    }
    return result;
}

string ArgumentParser::getInputLocation() {
    auto arguments = parse(argc, argv);
    for (int i = 0; i<arguments.size(); i++) {
        if (arguments[i] == "--input" || arguments[i] == "-i") {
            if (i == arguments.size() - 1) return ""; 
            else return arguments[i+1];
        }
    }
    return "";
}

string ArgumentParser::getOutputLocation() {
    if (parse(argc, argv).size() == 1) return "";
    else return parse(argc, argv).back();
}

vector<string> ArgumentParser::parse(int argc, char** argv) {
    vector<string> result; 
    for (int i = 0; i<argc; i++) {
        result.push_back(string(argv[i]));
    }
    return result;
}