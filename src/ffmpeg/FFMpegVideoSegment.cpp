
#include <ffmpeg/FFMpegVideoSegment.hpp>
#include <ffmpeg/FFMpegVideoFrame.hpp>
#include <utility>

#include <libavcodec/avcodec.h>
#include <iostream>

using namespace dtcode::ffmpeg; 
using namespace dtcode::data; 

using namespace std; 



FFMpegVideoSegment::FFMpegVideoSegment(AVCodecContext* ctx, AVPacket* pkt) {
    this->pkt = pkt;
    this->ctx = ctx;

    int ret = avcodec_send_packet(ctx, pkt);
    if (ret < 0) {
        // if (ret == AVERROR(EAGAIN)) {
            // nextFrame();
        // }
        // else {
            fprintf(stderr, "Error sending a packet for decoding\n");
            exit(1);
        // }
        
    }

    cout << "Parsed packet ";
    cout << "keyFrame= " << containsKeyFrame() 
        << " size= " << pkt->size
        << " data= " << pkt->data

        << endl;

}

std::optional<shared_ptr<Frame>> FFMpegVideoSegment::nextFrame() {
    
    AVFrame* frame = av_frame_alloc();
    int ret = avcodec_receive_frame(ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return optional<shared_ptr<Frame>>();
    else if (ret < 0) {
        fprintf(stderr, "Error during decoding\n");
        return optional<shared_ptr<Frame>>();
    }
    auto result = optional(make_shared<FFMpegVideoFrame>(*frame));
    av_frame_free(&frame);
    return result;
}

bool FFMpegVideoSegment::containsKeyFrame() {
    return pkt->flags & AV_PKT_FLAG_KEY;
}
