#include <ffmpeg/utils.hpp>
#include <ffmpeg/FFMpegVideoFrame.hpp>
#include <ffmpeg/FFMpegVideoStream.hpp>

#include <vector>

using namespace std; 

using namespace dtcode::data;

namespace dtcode::ffmpeg::utils {

int open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type) {
    int ret, stream_index;
    AVStream *st;
    const AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input\n",
                av_get_media_type_string(type));
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}


int get_format_from_sample_fmt(const char **fmt, AVSampleFormat sample_fmt) {
        int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;   
}


std::list<dtcode::data::StreamPtr> get_stream_from_file(std::string filename) {
    int ret = 0;
    AVFormatContext* fmt_ctx = NULL;

    auto src_filename = filename.c_str();
    /* open input file, and allocate format context */
    printf("Opening %s\n", src_filename);   
    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        exit(1);
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
    AVStream *video_stream = NULL, *audio_stream = NULL;
    int video_stream_idx = -1, audio_stream_idx = -1;


    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];


        /* allocate image where the decoded image will be put */
        auto width = video_dec_ctx->width;
        auto height = video_dec_ctx->height;
        auto pix_fmt = video_dec_ctx->pix_fmt;
    }

    // if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
    //     audio_stream = fmt_ctx->streams[audio_stream_idx];
    // }

    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, src_filename, 0);

    if (!audio_stream && !video_stream) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        ret = 1;
        exit(1);
    }

    auto frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        exit(1);
    }

    auto pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate packet\n");
        ret = AVERROR(ENOMEM);
        exit(1);
    }


    vector<uint8_t> video_data, audio_data;
    /* read frames from the file */
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise
        // skip it
        if (pkt->stream_index == video_stream_idx)
            video_data.insert(video_data.end(), pkt->data, pkt->data + pkt->size);
        else if (pkt->stream_index == audio_stream_idx)
            // video_data.insert(video_data.end(), pkt->data, pkt->data + pkt->size);
            ;
        av_packet_unref(pkt);
        if (ret < 0)
            break;
    }

    // if (audio_stream) {
    //     enum AVSampleFormat sfmt = audio_dec_ctx->sample_fmt;
    //     int n_channels = audio_dec_ctx->channels;
    //     const char *fmt;

    //     if (av_sample_fmt_is_planar(sfmt)) {
    //         const char *packed = av_get_sample_fmt_name(sfmt);
    //         printf("Warning: the sample format the decoder produced is planar "
    //                "(%s). This example will output the first channel only.\n",
    //                packed ? packed : "?");
    //         sfmt = av_get_packed_sample_fmt(sfmt);
    //         n_channels = 1;
    //     }

    //     if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
    //         goto end;

    //     printf("Play the output audio file with the command:\n"
    //            "ffplay -f %s -ac %d -ar %d %s\n",
    //            fmt, n_channels, audio_dec_ctx->sample_rate,
    //            audio_dst_filename);
    // }


    avcodec_free_context(&video_dec_ctx);
    // avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    return list<StreamPtr> {
        make_shared<FFMpegVideoStream>(video_data)
    };

}

}