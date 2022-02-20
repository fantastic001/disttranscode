
#pragma once 

#include <video/VideoWriter.hpp>
#include <ffmpeg/FFMpegVideoSegment.hpp>

#include <string>


extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>

    #include <libavcodec/avcodec.h>

}

namespace dtcode::ffmpeg {
    class FFMpegVideoEncoder : public dtcode::video::VideoWriter {
            std::list<dtcode::data::FramePtr> frames; 
            void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, std::shared_ptr<dtcode::ffmpeg::FFMpegVideoSegment> segment);
        public:
            FFMpegVideoEncoder();
            ~FFMpegVideoEncoder();
            void writeFrame(dtcode::data::FramePtr frame);
            std::shared_ptr<dtcode::ffmpeg::FFMpegVideoSegment> getSegment();
    };
}