
#pragma once 

#include <data/Segment.hpp>
extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>

    #include <libavcodec/avcodec.h>
}

#include <list>
#include <utility>
namespace dtcode::ffmpeg {
    class FFMpegVideoSegment : public dtcode::data::Segment {
            AVCodecContext* ctx;
            bool hasKeyFrame;
            std::list<std::pair<AVPacket*, bool>> packets;
            std::list<std::pair<AVPacket*, bool>>::iterator it;
        public:
            FFMpegVideoSegment(AVCodecContext* ctx);
            ~FFMpegVideoSegment();
            std::optional<std::shared_ptr<dtcode::data::Frame>> nextFrame();
            bool containsKeyFrame();
            void addPacket(AVPacket* pkt, bool keyFrame);
            std::list<dtcode::data::FramePtr> decodeKeyFrames();

    };
}