
#include "ffmpeg/FFMpegVideoFrame.hpp"


using namespace dtcode::data;
using namespace dtcode::ffmpeg;
using namespace std; 


FFMpegVideoFrame::FFMpegVideoFrame(AVFrame* frame) {
    this->frame = frame;
}

FFMpegVideoFrame::~FFMpegVideoFrame() {
    av_frame_free(&frame);
}

int FFMpegVideoFrame::getChannelCount() {
    return 3;
}

vector<int> FFMpegVideoFrame::getDim() {
    return vector<int> {frame->height, frame->width};
}

unsigned char FFMpegVideoFrame::getData(int channel,const std::vector<int>& position) {
    return *(frame->data[channel] + position[0]*frame->linesize[channel] + position[1]);
}

bool FFMpegVideoFrame::isKeyFrame() {
    return frame->key_frame == 1;
}