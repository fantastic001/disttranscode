
#pragma once 

#include <net/DistributionFactory.hpp>
#include <net/ConsensusFactory.hpp>
#include <data/Filter.hpp>
#include <memory>

namespace dtcode::net {
    class Synchronizer {
            std::shared_ptr<ConsensusFactory> consensusFactory;
            std::shared_ptr<DistributionFactory> distributionFactory;
            std::list<dtcode::data::FilterPtr> filters; 
            bool root;
        public:
            Synchronizer(
                std::shared_ptr<DistributionFactory> distributionFactory, 
                std::shared_ptr<ConsensusFactory> consensusFactory,
                bool root = true
            );

            std::vector<dtcode::data::SegmentPtr> process(dtcode::data::StreamPtr stream);
            void addFilter(::dtcode::data::FilterPtr filter);

    };
}