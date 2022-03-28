
#pragma once 

#include <memory>
#include <net/Consensus.hpp>
#include <vector>
#include <optional>

namespace dtcode::net {
    template<class T>
    class UniformFloodingConsensus : public Consensus {
            std::shared_ptr<T> context;
            std::vector<int> proposals;
            int nextDecision;
        public:
            UniformFloodingConsensus(std::shared_ptr<T> context) {
                this->context = context;
                nextDecision = 0;
            }
            void propose(int index) {

            }
            int getDecision() {
                auto result = nextDecision;
                nextDecision++;
                return result;
            }
    };
}