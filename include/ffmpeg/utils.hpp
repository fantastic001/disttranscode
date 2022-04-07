#pragma once 

extern "C" {

    // #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>

}

#include <list>
#include <string>
#include <data/Frame.hpp>
#include <data/Stream.hpp>


namespace dtcode::ffmpeg::utils 
{

    int open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);

    int get_format_from_sample_fmt(const char **fmt, AVSampleFormat sample_fmt);

    std::list<dtcode::data::StreamPtr> get_stream_from_file(std::string filename);

}