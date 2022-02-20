#pragma once 

#include <net/Distribution.hpp>

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
            void distribute(dtcode::data::StreamPtr stream) {
                if (stream == nullptr) return;
                auto segments_list = stream->parse();
                for (auto segment : segments_list) {
                    segments.push_back(segment);
                }
                if (segments.size() == 0) return;
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
            }
            int getRank(int index) {
                return index % context->size();
            }
            int nextIndex() {
                if (next == segments.size()) return -1;
                else return next++;
            }

    };
}