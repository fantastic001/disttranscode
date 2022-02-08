
#include <data/FilterManager.hpp>


using namespace dtcode::data;
using namespace std; 

FilterManager::FilterManager() {

}

bool FilterManager::registerFilter(std::string name, FilterArgumentParserPtr parser) {
    parsers[name] = parser;
}

FilterArgumentParserPtr FilterManager::findFilter(string name) {
    auto it = parsers.find(name);
    if (it == parsers.end()) return nullptr;
    else return it->second;
}

std::shared_ptr<FilterManager> FilterManager::instance = nullptr;

shared_ptr<FilterManager> FilterManager::getInstance() {
    if (!instance) {
        instance = shared_ptr<FilterManager>(new FilterManager());
    }
    return instance;
}

list<string> FilterManager::getAllFilterNames() {
    list<string> result;
    for (auto it : parsers) {
        result.push_back(it.first);
    }
    return result;
}