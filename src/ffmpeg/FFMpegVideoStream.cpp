

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

    f = fopen(filename.c_str(), "rb");
    if (!f) {
        // TODO throw exception here
        fprintf(stderr, "Could not open %s\n", filename.c_str());
        exit(1);
    }

}

FFMpegVideoStream::~FFMpegVideoStream() {
    fclose(f);
    av_parser_close(parser);
    avcodec_free_context(&c);
}

std::list<dtcode::data::SegmentPtr> FFMpegVideoStream::parse() {
    list<SegmentPtr> segments; 
    shared_ptr<FFMpegVideoSegment> current_segment = nullptr;
    while (!feof(f)) {
        /* read raw data from the input file */
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (!data_size)
            break;

        /* use the parser to split the data into frames */
        data = inbuf;
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
        }
        if (current_segment) {
            segments.push_back(current_segment);
        }
    }
    return static_cast<list<SegmentPtr>>(segments);
}