
#include <ffmpeg/FFMpegVideoSegment.hpp>
#include <ffmpeg/FFMpegVideoFrame.hpp>
#include <utility>

#include <libavcodec/avcodec.h>
#include <iostream>

using namespace dtcode::ffmpeg; 
using namespace dtcode::data; 

using namespace std; 



FFMpegVideoSegment::FFMpegVideoSegment(AVCodecContext* ctx) {
    this->ctx = ctx;
    it = packets.begin();
}

FFMpegVideoSegment::~FFMpegVideoSegment() {
    for (auto it : packets) {
        av_packet_free(&it.first);
    }
}


bool FFMpegVideoSegment::containsKeyFrame() {
    return hasKeyFrame;
}

std::optional<shared_ptr<Frame>> FFMpegVideoSegment::nextFrame() {
    if (it == packets.end()) return optional<FramePtr>();
    AVPacket* pkt = it->first;
    int ret = avcodec_send_packet(ctx, pkt);
    if (ret < 0) {
        if (ret == AVERROR(EAGAIN)) {
            return nextFrame();
        }
        else {
            fprintf(stderr, "Error sending a packet for decoding\n");
            return optional<shared_ptr<FFMpegVideoFrame>>();
        }
        
    }
    AVFrame* frame = av_frame_alloc();
    ret = avcodec_receive_frame(ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return optional<FramePtr>();
    else if (ret < 0) {
        fprintf(stderr, "Error during decoding\n");
        return optional<FramePtr>();
    }
    auto result = optional<shared_ptr<FFMpegVideoFrame>>(make_shared<FFMpegVideoFrame>(frame));
    // TODO find better way to deallocate frames such that data in previous frames is not affected
    // av_frame_free(&frame);
    it++;
    return result;

}
void FFMpegVideoSegment::addPacket(AVPacket* packet, bool keyFrame) {
    packets.push_back(make_pair(packet, keyFrame));
    hasKeyFrame |= keyFrame;
    it = packets.begin();
    serialized.insert(serialized.end(), packet->data, packet->data + packet->size);
}


std::list<FramePtr> FFMpegVideoSegment::decodeKeyFrames() {
    list<FramePtr> result;
    for (auto kit = packets.begin(); kit != packets.end(); kit++) {
        auto packet = kit->first;
        auto isKeyFrame = kit->second;
        auto temp = it;
        it = kit; 
        auto maybe_frame = nextFrame();
        if (maybe_frame.has_value()) {
            result.push_back(maybe_frame.value());
        }
        it = temp;
    }
    return result;
}

vector<uint8_t> FFMpegVideoSegment::serialize() {
    auto data = serialized;
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x01);
    data.push_back(0xb7);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    return data;
}

int FFMpegVideoSegment::getFrameCount() {
    return packets.size();
}