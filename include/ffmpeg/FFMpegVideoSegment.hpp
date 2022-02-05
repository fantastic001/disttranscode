
#pragma once 

#include <data/Segment.hpp>
extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>

    #include <libavcodec/avcodec.h>

}
namespace dtcode::ffmpeg {
    class FFMpegVideoSegment : public dtcode::data::Segment {

            AVPacket* pkt;
            AVCodecContext* ctx;
            std::list<dtcode::data::FramePtr> frames;
            std::list<dtcode::data::FramePtr>::iterator it;
            dtcode::data::FramePtr read();
        public:
            FFMpegVideoSegment(AVCodecContext* ctx, AVPacket* pkt);
            std::optional<std::shared_ptr<dtcode::data::Frame>> nextFrame();
            bool containsKeyFrame();

    };
}