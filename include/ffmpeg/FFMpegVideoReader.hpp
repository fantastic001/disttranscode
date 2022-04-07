#pragma once 

#include <string>
#include <video/VideoReader.hpp>

namespace dtcode::ffmpeg {
    class FFMpegVideoReader : public dtcode::video::VideoReader{
        std::string path;
    public:
        FFMpegVideoReader(std::string path);

        dtcode::data::StreamPtr getVideoStream();
        dtcode::data::StreamPtr getAudioStream();
    };
}