
#pragma once 

#include <list>
#include <data/Segment.hpp>

namespace dtcode::data {
    class Stream {
        public:
            virtual std::list<SegmentPtr> parse() =0;
    };
}