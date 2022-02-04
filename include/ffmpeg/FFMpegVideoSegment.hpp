
#pragma once 

#include <data/Segment.hpp>
extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>

    #include <libavcodec/avcodec.h>

}
namespace dtcode::ffmpeg {
    class FFMpegVideoSegment : public dtcode::data::Segment {

            AVPacket pkt;
            AVCodecContext* ctx; 

        public:
            FFMpegVideoSegment(AVCodecContext* ctx, AVPacket pkt);
            std::optional<std::shared_ptr<dtcode::data::Frame>> nextFrame();
            bool containsKeyFrame();

    };
}