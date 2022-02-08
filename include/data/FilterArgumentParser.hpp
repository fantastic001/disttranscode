
#pragma once 


#include <string>
#include <data/FilterFactory.hpp>

namespace dtcode::data {
    class FilterArgumentParser {
        public:
            virtual FilterFactoryPtr parse(
                std::vector<std::string>::iterator start,
                std::vector<std::string>::iterator end
            )=0;
    };
    using FilterArgumentParserPtr = std::shared_ptr<FilterArgumentParser>;
}

#define REGISTER_FILTER(name, parser) struct Reg_##name {static bool X;}; \
    bool Reg_##name::X = dtcode::data::FilterManager::getInstance()->registerFilter(#name, std::make_shared< parser >());