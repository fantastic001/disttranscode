
#pragma once 

#include <data/Frame.hpp>

extern "C" {
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>

    #include <libavcodec/avcodec.h>

}
namespace dtcode::ffmpeg {
    class FFMpegVideoFrame : public dtcode::data::Frame {
            AVFrame* frame;
        public:

            FFMpegVideoFrame(AVFrame* frame);
            ~FFMpegVideoFrame();

            int getChannelCount();
            std::vector<int> getDim();
            unsigned char getData(int channel,const std::vector<int>& position);

            bool isKeyFrame();
    };
}