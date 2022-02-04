
#pragma once 

#include <vector>
#include <memory>


namespace dtcode::data {
    class Frame {
        public:
            virtual int getChannelCount() = 0;
            virtual std::vector<int> getDim() =0;
            virtual unsigned char getData(int channel,std::vector<int> position) =0;
            virtual bool isKeyFrame() =0;
    };

    using FramePtr = std::shared_ptr<Frame>;
    std::vector<int> pos2d(int row, int column);
    std::vector<int> pos1d(int pos);
}