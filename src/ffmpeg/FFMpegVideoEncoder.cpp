
#include <ffmpeg/FFMpegVideoEncoder.hpp>
#include <ffmpeg/FFMpegVideoSegment.hpp>

extern "C" {
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>

}

#include <iostream>

using namespace dtcode::ffmpeg;
using namespace dtcode::data;
using namespace std;


FFMpegVideoEncoder::FFMpegVideoEncoder() {
}

FFMpegVideoEncoder::~FFMpegVideoEncoder() {
}

void FFMpegVideoEncoder::writeFrame(FramePtr m_frame) {
    frames.push_back(m_frame);
    
}

void FFMpegVideoEncoder::encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, shared_ptr<FFMpegVideoSegment> segment) {
    int ret;

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN))
        {
            return;
        }
        else if (ret < 0) {
            return;
        }
        segment->addPacket(pkt, frame == NULL ? false : frame->pict_type == AV_PICTURE_TYPE_I);
    }
}

shared_ptr<FFMpegVideoSegment> FFMpegVideoEncoder::getSegment() {
    int i = 0;
    auto codec = avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    auto c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    auto segment = make_shared<FFMpegVideoSegment>(c);
        /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    if (frames.size() == 0) return segment;
    c->width = frames.front()->getDim()[1];
    c->height = frames.front()->getDim()[0];
    /* frames per second */
    c->time_base = (AVRational){1, 30};
    c->framerate = (AVRational){30, 1};

    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    auto ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    for (auto m_frame : frames) {
        auto pkt = av_packet_alloc();
        if (!pkt)
            exit(1);
        AVFrame* frame;
        frame = av_frame_alloc();
        if (!frame) {
            fprintf(stderr, "Could not allocate video frame\n");
            exit(1);
        }
        frame->format = c->pix_fmt;
        frame->width  = c->width;
        frame->height = c->height;

        auto ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate the video frame data\n");
            exit(1);
        }
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);
        
        #pragma omp parallel for 
        for (int channel = 0; channel < m_frame->getChannelCount(); channel++) {
            // optimization - allocating vector every time we call pos2d is too expensive 
            std::vector<int> position;
            position.resize(2);
            int k = channel == 0 ? 1 : 2;
            for (position[0] = 0; position[0] < c->height/k; position[0]++) {
                for (position[1] = 0; position[1] < c->width/k; position[1]++) {
                    frame->data[channel][position[0] * frame->linesize[channel] + position[1]] = m_frame->getData(channel, position);
                }
            }
        }


        frame->pts = i;
        if (m_frame->isKeyFrame()) {
            frame->pict_type = AV_PICTURE_TYPE_I;
        }
        i++;
        encode(c, frame, pkt, segment);
    }
    // // finalize 
    auto pkt = av_packet_alloc();
    encode(c, NULL, pkt, segment);
    avcodec_free_context(&c);

    return segment;
}