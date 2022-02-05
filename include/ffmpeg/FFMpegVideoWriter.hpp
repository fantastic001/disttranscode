
#pragma once 

#include <video/VideoWriter.hpp>
#include <string>


extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>

    #include <libavcodec/avcodec.h>

}

namespace dtcode::ffmpeg {
    class FFMpegVideoWriter : public dtcode::video::VideoWriter {
            const AVCodec *codec;
            AVCodecContext *c= NULL;
            int i, ret, x, y;
            FILE *f;
            AVFrame *frame;
            AVPacket *pkt;
            uint8_t *endcode;
            void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile);

            bool initialized;

        public:
            FFMpegVideoWriter(std::string filename, std::string codec_name);
            ~FFMpegVideoWriter();
            void writeFrame(dtcode::data::FramePtr frame);
    };
}