
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
        public:
            UniformFloodingConsensus(std::shared_ptr<T> context) {
                this->context = context;
            }
            void propose(int index) {
                (*context)() << "PROPOSAL " << index;
                proposals.push_back(index);
            }
            int getDecision() {
                for (int i = 0; i<context->size(); i++) {
                    std::string msg;
                    int proposedBy;
                    if (i != context->rank()) {
                        (*context)[i] >> msg >> proposedBy;
                        proposals.push_back(proposedBy);
                    }
                }
                auto result = proposals[0];
                for (auto p : proposals) {
                    if (result > p) {
                        result = p;
                    }
                }
                return result;
            }
    };
}