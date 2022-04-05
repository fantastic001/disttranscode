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
            std::vector<int> accumulated_frame_count;
        public:
            
            StrideDistribution(std::shared_ptr<T> context) {
                this->context = context;
                next = -1;
            }

            virtual int getFrameCountBeforeThisSegment(int segment_index) {
                return accumulated_frame_count[segment_index]; 
            }
            dtcode::data::StreamPtr distribute(dtcode::data::StreamPtr stream) {
                next = context->rank();
                if (context->rank() == 0) {
                    if (stream == nullptr) return nullptr;
                    int segment_index = 0;
                    auto segments_list = stream->parse();
                    for (auto segment : segments_list) {
                        int frameCount = segment_index == 0 ? 0 : segments.back()->getFrameCount();
                        accumulated_frame_count.push_back(
                            segment_index == 0 ? 0 : accumulated_frame_count[segment_index - 1] +  frameCount
                        );
                        segments.push_back(segment);
                        segment_index++;
                    }
                    if (segments.size() == 0) return stream;
                    (*context)() << stream->getData();
                }
                else {
                    std::vector<uint8_t> data_packets;
                    (*context)[0] >> data_packets;
                    stream = std::make_shared<dtcode::ffmpeg::FFMpegVideoStream>(data_packets);
                    auto segment_list = stream->parse();
                    for (auto seg : segment_list) {
                        segments.push_back(seg);
                    }
                    accumulated_frame_count.resize(segments.size());
                }
                for (int i = 0; i<accumulated_frame_count.size(); i++) {
                    context->share(0, accumulated_frame_count[i]);
                }
                return stream;
            }
            int getRank(int index) {
                return index % context->size();
            }
            int nextIndex() {
                if (next >= segments.size()) return -1;
                else {
                    int tmp = next;
                    next += context->size();
                    return tmp;
                }
            }
            dtcode::data::SegmentPtr getSegment(int index, dtcode::data::SegmentPtr toSend = nullptr) {
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
                    if (toSend == nullptr) {
                        auto my_segment = segments[index];
                        return my_segment;
                    }
                    else {
                        (*context)() << toSend->serialize();
                        return toSend;
                    }
                }
            }

    };
}