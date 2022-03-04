
#pragma once 

#include <data/Stream.hpp>

namespace dtcode::net {
    class Distribution {
        public:
            virtual dtcode::data::StreamPtr distribute(dtcode::data::StreamPtr stream) =0;
            virtual int getRank(int segmentIndex) =0;
            virtual int nextIndex() =0;
            virtual dtcode::data::SegmentPtr getSegment(int index, dtcode::data::SegmentPtr toSend = nullptr) =0;
    };
    using DistributionPtr = std::shared_ptr<Distribution>;
}