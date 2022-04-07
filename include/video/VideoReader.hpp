
#pragma once 

#include <data/Stream.hpp>

#include <memory>

namespace dtcode::video {
    class VideoReader {
        public:
            virtual dtcode::data::StreamPtr getVideoStream() =0;
            virtual dtcode::data::StreamPtr getAudioStream() =0;
    };
    using VideoReaderPtr = std::shared_ptr<VideoReader>;
}