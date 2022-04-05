
#pragma once 

#include "Frame.hpp"
#include <optional>
#include <list>
#include <memory>

namespace dtcode::data {

    class Segment {
        public:
            virtual std::optional<std::shared_ptr<Frame>> nextFrame() =0;
            virtual bool containsKeyFrame() =0;
            virtual std::list<FramePtr> decodeKeyFrames() =0;
            virtual std::vector<uint8_t> serialize() =0;
            virtual int getFrameCount();

            std::list<std::shared_ptr<Frame>> decodeAllFrames();
    };

    using SegmentPtr = std::shared_ptr<Segment>;
}