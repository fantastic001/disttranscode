#pragma once 

#include <net/Consensus.hpp>
#include <memory>

namespace dtcode::net {
    class ConsensusFactory {
        public:
            virtual std::shared_ptr<Consensus> create() = 0;
    };
}