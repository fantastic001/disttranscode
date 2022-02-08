
#pragma once 

#include <data/Filter.hpp>
#include <map>
#include <string>
#include <sstream>

namespace dtcode::data {
    class FilterFactory {
            std::map<std::string, std::string> params;
        public:
            template<class T> 
            void setParameter(std::string key, T value) {
                std::stringstream ss;
                ss << value;
                params[key] = ss.str();
            }

            template<class T>
            T getParameter(std::string key, T defaulotValue) {
                auto it = params.find(key);
                if (it == params.end()) {
                    return defaulotValue;
                }
                else {
                    std::stringstream ss(it->second); 
                    T result; 
                    ss >> result; 
                    return result;
                }
            }

            virtual FilterPtr create() = 0;
    };
    using FilterFactoryPtr = std::shared_ptr<FilterFactory>;
}