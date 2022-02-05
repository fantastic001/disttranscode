
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

    constexpr unsigned char rgb2yuv(double r, double g, double b, int channel) {
        return (unsigned char) (255 * (
            (0.299 * r + 0.587*g + 0.114*b) * (channel == 0) * 1 +
            (0.436 + 0.492 * (b - 0.299 * r - 0.587*g - 0.114*b)) * (channel == 1) / (2*0.436) +
            (0.615 + 0.877 * (r - 0.299 * r - 0.587*g - 0.114*b)) * (channel == 2) / (2*0.615)
        ));
    }
}