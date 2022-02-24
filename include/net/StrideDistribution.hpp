#pragma once 

#include <net/Distribution.hpp>
#include <ffmpeg/FFMpegVideoStream.hpp>
#include <ffmpeg/FFMpegVideoEncoder.hpp>
#include <frame/FrameF.hpp>

namespace dtcode::net {
    template<class T>
    class StrideDistribution : public Distribution {
            std::shared_ptr<T> context;
            int next;
            std::vector<dtcode::data::SegmentPtr> segments;
        public:
            
            StrideDistribution(std::shared_ptr<T> context) {
                this->context = context;
                next = -1;
            }
            std::list<dtcode::data::SegmentPtr> distribute(dtcode::data::StreamPtr stream) {
                if (context->rank() == 0) {
                    if (stream == nullptr) return std::list<dtcode::data::SegmentPtr>();
                    auto segments_list = stream->parse();
                    for (auto segment : segments_list) {
                        segments.push_back(segment);
                    }
                    if (segments.size() == 0) return segments_list;
                    for (int i = 1; i<context->size(); i++) {
                        (*context)[i] << segments.size();
                    }
                    next = 0;
                    for (int i = 0; i<segments.size(); i++) {
                        auto data = segments[i]->serialize();
                        int rank = i % context->size();
                        if (rank != context->rank()) {
                            (*context)[rank] << data;
                            if (i < segments.size() - 1) 
                            {
                                (*context)[rank] << segments[i+1]->serialize();
                            }
                        }
                    }
                    return segments_list;
                }
                else {
                    int size; 
                    (*context)[0] >> size;
                    for (int i = context->rank(); i<size; i += context->size()) {
                        std::vector<uint8_t> data, next_data;
                        (*context)[0] >> data;
                        (*context)[0] >> next_data;
                        auto parsed = std::make_shared<dtcode::ffmpeg::FFMpegVideoStream>(data)->parse();
                        if (parsed.size() > 0) segments.push_back(parsed.front());
                        else {
                            // well, let's make dummy segment :(
                            auto encoder = dtcode::ffmpeg::FFMpegVideoEncoder();
                            encoder.writeFrame(dtcode::frame::F(10, 10, [] (int c, int y, int x) {
                                return 0;
                            }));
                            segments.push_back(encoder.getSegment());
                        }
                    }
                    return std::list<dtcode::data::SegmentPtr>();
                }
            }
            int getRank(int index) {
                return index % context->size();
            }
            int nextIndex() {
                if (next == segments.size()) return -1;
                else return next++;
            }
            dtcode::data::SegmentPtr getSegment(int index) {
                if (getRank(index) != context->rank()) {
                    std::vector<uint8_t> data; 
                    (*context)[getRank(index)] >> data;
                    auto parsed = std::make_shared<dtcode::ffmpeg::FFMpegVideoStream>(data)->parse();
                    if (parsed.size() == 0) {
                        // well, let's make dummy segment :(
                        auto encoder = dtcode::ffmpeg::FFMpegVideoEncoder();
                        encoder.writeFrame(dtcode::frame::F(10, 10, [] (int c, int y, int x) {
                            return 0;
                        }));
                        return encoder.getSegment();
                    }
                    else {
                        return parsed.front();
                    }
                }
                else {
                    auto my_segment = segments[(index - context->rank()) / context->size()];
                    (*context)() << my_segment->serialize();
                    return my_segment;
                }
            }

    };
}