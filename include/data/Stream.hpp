
#pragma once 

#include <list>
#include <data/Segment.hpp>
#include <vector>
#include <memory>

namespace dtcode::data {
    class Stream {
        public:
            virtual std::list<SegmentPtr> parse() =0;
            virtual std::vector<uint8_t> getData() = 0;
    };
    using StreamPtr = std::shared_ptr<Stream>;
}