
#pragma once 

#include <data/Frame.hpp>

namespace dtcode::video {
    class VideoWriter {

        public:
            virtual void writeFrame(dtcode::data::FramePtr frame) =0;
    };
}