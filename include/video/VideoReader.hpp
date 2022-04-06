
#pragma once 

#include <data/Stream.hpp>

namespace dtcode::video {
    class VideoReader {
        virtual dtcode::data::Stream getStream() =0;
    };
}