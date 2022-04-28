
#include <ffmpeg/FFMpegVideoWriter.hpp>

extern "C" {
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>

}


using namespace dtcode::ffmpeg;
using namespace dtcode::data;
using namespace std;


FFMpegVideoWriter::FFMpegVideoWriter(std::string filename, std::string codec_name) {
    endcode = new uint8_t[4] { 0, 0, 1, 0xb7 };
    i = 0;
    codec = avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
    // codec = avcodec_find_encoder_by_name(codec_name.c_str());
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);
    initialized = false;
    f = fopen(filename.c_str(), "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
}

FFMpegVideoWriter::~FFMpegVideoWriter() {
    if (initialized) {
        encode(c, NULL, pkt, f);
        if (codec->id == AV_CODEC_ID_MPEG1VIDEO || codec->id == AV_CODEC_ID_MPEG2VIDEO)
            fwrite(endcode, 1, sizeof(endcode), f);    fclose(f);

        avcodec_free_context(&c);
        av_frame_free(&frame);
        av_packet_free(&pkt);
    }
    

}

void FFMpegVideoWriter::writeFrame(FramePtr m_frame) {
    if (!initialized) {
        initialized = true;
        /* put sample parameters */
        c->bit_rate = 400000;
        /* resolution must be a multiple of two */
        c->width = m_frame->getDim()[1];
        c->height = m_frame->getDim()[0];
        /* frames per second */
        c->time_base = (AVRational){1, 30};
        c->framerate = (AVRational){30, 1};

        /* emit one intra frame every ten frames
        * check frame pict_type before passing frame
        * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
        * then gop_size is ignored and the output of encoder
        * will always be I frame irrespective to gop_size
        */
        c->gop_size = 10;
        c->max_b_frames = 1;
        c->pix_fmt = AV_PIX_FMT_YUV420P;

        if (codec->id == AV_CODEC_ID_H264)
            av_opt_set(c->priv_data, "preset", "slow", 0);

        /* open it */
        ret = avcodec_open2(c, codec, NULL);
        if (ret < 0) {
            fprintf(stderr, "Could not open codec\n");
            exit(1);
        }

        frame = av_frame_alloc();
        if (!frame) {
            fprintf(stderr, "Could not allocate video frame\n");
            exit(1);
        }
        frame->format = c->pix_fmt;
        frame->width  = c->width;
        frame->height = c->height;

        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate the video frame data\n");
            exit(1);
        }

    }



    ret = av_frame_make_writable(frame);
    if (ret < 0)
        exit(1);

    /* Prepare a dummy image.
        In real code, this is where you would have your own logic for
        filling the frame. FFmpeg does not care what you put in the
        frame.
        */
    /* Y */
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
    encode(c, frame, pkt, f);
}

void FFMpegVideoWriter::encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile) {
    int ret;

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error %d sending a frame for encoding\n", ret);
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        // printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

void FFMpegVideoWriter::writeSegment(SegmentPtr segment) {
    for (uint8_t byte : segment->serialize()) {
        fwrite(&byte, 1, 1, f);
    }
}