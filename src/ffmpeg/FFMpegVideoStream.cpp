

#include <ffmpeg/FFMpegVideoStream.hpp>
#include <cstdio>
#include <cstdlib>
#include <ffmpeg/FFMpegVideoSegment.hpp>

#include <iostream>

using namespace dtcode::ffmpeg;
using namespace dtcode::data;
using namespace std; 

FFMpegVideoStream::FFMpegVideoStream(std::string filename) {

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

   

    FILE* f = fopen(filename.c_str(), "rb");
    if (!f) {
        // TODO throw exception here
        fprintf(stderr, "Could not open %s\n", filename.c_str());
        exit(1);
    }
    while (!feof(f)) {
        auto count = fread(inbuf, 1, INBUF_SIZE, f);
        if (!count) break;
        serialized_data.insert(serialized_data.end(), inbuf, inbuf+count);
    }
    fclose(f);
}

FFMpegVideoStream::FFMpegVideoStream(vector<uint8_t> data) {
    serialized_data = data;
}

FFMpegVideoStream::~FFMpegVideoStream() {
    av_parser_close(parser);
    avcodec_free_context(&c);
}

std::list<dtcode::data::SegmentPtr> FFMpegVideoStream::parse() {
    /* find the MPEG-1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if (!codec) {
        // TODO throw exception here
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        // TODO throw exception here
        fprintf(stderr, "parser not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        // TODO throw exception here
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        // TODO throw exception here
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    list<SegmentPtr> segments; 

    shared_ptr<FFMpegVideoSegment> current_segment = nullptr;
    data_size = serialized_data.size();
    data = new uint8_t[data_size];
    copy(serialized_data.begin(), serialized_data.end(), data);
    while (data_size > 0) {
        auto pkt = av_packet_alloc();
        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                                data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            // TODO throw exception here
            fprintf(stderr, "Error while parsing\n");
            exit(1);
        }
        data      += ret;
        data_size -= ret;

        if (pkt->size) {
            if (parser->pict_type == AV_PICTURE_TYPE_I) {
                if (current_segment) {
                    segments.push_back(current_segment);
                }
                current_segment = make_shared<FFMpegVideoSegment>(c);
                current_segment->addPacket(pkt, true);
            }
            else {
                if (!current_segment) {
                    current_segment = make_shared<FFMpegVideoSegment>(c);
                    current_segment->addPacket(pkt, false);
                }
                else {
                    current_segment->addPacket(pkt, false);
                }
            }
        }
        if (current_segment) {
            segments.push_back(current_segment);
        }
    }
    return static_cast<list<SegmentPtr>>(segments);
}