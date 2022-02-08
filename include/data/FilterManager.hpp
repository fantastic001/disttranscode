
#pragma once

#include <data/FilterArgumentParser.hpp>
#include <list>

namespace dtcode::data {
    class FilterManager {
            std::map<std::string, FilterArgumentParserPtr> parsers;
            FilterManager();

            static std::shared_ptr<FilterManager> instance; 
         public:
            FilterArgumentParserPtr findFilter(std::string name);
            bool registerFilter(std::string name, FilterArgumentParserPtr parser);
            static std::shared_ptr<FilterManager> getInstance();

            std::list<std::string> getAllFilterNames();
    };
}