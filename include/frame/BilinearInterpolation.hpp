#pragma once 

#include <data/Frame.hpp>

namespace dtcode {
    namespace frame {
        class BilinearInterpolation {
                ::dtcode::data::FramePtr frame;
            public:
                BilinearInterpolation(::dtcode::data::FramePtr frame);
                uint8_t at(int channel, double x, double y);
        };
    }
}