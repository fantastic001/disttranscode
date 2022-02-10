#pragma once

#include <data/Stream.hpp>
#include <string>
#include <list>
extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>

    #include <libavcodec/avcodec.h>

}
#define INBUF_SIZE 1024


namespace dtcode::ffmpeg {
    class FFMpegVideoStream : public ::dtcode::data::Stream {
        AVCodec *codec;
        AVCodecParserContext *parser;
        AVCodecContext *c= NULL;
        std::vector<uint8_t> serialized_data;
        AVFrame *frame;
        uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
        uint8_t *data;
        size_t   data_size;
        int ret;

        public:
            FFMpegVideoStream(std::string filename);
            FFMpegVideoStream(std::vector<uint8_t> serialized_data);
            ~FFMpegVideoStream();
            std::list<dtcode::data::SegmentPtr> parse();
    };
}