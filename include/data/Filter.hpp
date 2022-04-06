
#pragma once 

#include <data/Frame.hpp>
#include <memory>

namespace dtcode::data {
    class Filter {
        public:
            virtual FramePtr filter(FramePtr frame, int frameNumber) =0;
    };
    using FilterPtr = std::shared_ptr<Filter>;
}