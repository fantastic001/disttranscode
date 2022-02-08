
#pragma once

#include <string>
#include <vector>

#include <data/Filter.hpp>


namespace dtcode::cmd {
    class ArgumentParser {
            std::vector<std::string> parse(int argc, char** argv);
            int argc; 
            char** argv;
        public:
            ArgumentParser(int argc, char** argv);
            std::string getInputLocation();
            std::string getOutputLocation();
            std::vector<dtcode::data::FilterPtr> getFilters();
    };
}

