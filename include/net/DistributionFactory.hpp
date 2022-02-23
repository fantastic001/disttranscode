
#pragma once 

#include <net/Distribution.hpp>
#include <memory>

namespace dtcode::net {
    class DistributionFactory {
        public:
            virtual std::shared_ptr<Distribution> create() = 0;
    };
}