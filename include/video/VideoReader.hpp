
#pragma once 

#include <data/Stream.hpp>

namespace dtcode::video {
    class VideoReader {
        dtcode::data::Stream getStream() =0;
    };
}