
#pragma once 

#include <data/Frame.hpp>

namespace dtcode::frame {
    class FrameGenerator {
        public:
            virtual dtcode::data::FramePtr generateFrame(int frameNumber) =0;
    };

    using FrameGeneratorPtr = std::shared_ptr<FrameGenerator>;
}