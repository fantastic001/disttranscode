#pragma once 

#include <memory>

namespace dtcode::net {
    class Consensus {
        public:
            virtual void propose(int index)=0;
            virtual int getDecision() =0;
    };
    using ConsensusPtr = std::shared_ptr<Consensus>;
}