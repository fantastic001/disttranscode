#pragma once 

#include <data/Frame.hpp>
#include <functional>


namespace dtcode::frame {
    template<typename F>
    class FrameF : public dtcode::data::Frame {
            unsigned char *data;
            int height, width;
            std::vector<int> dim; 
            std::function<unsigned char (int, int, int)> f;
        public:
            FrameF(int height, int width,const F& f) {
                this->f = f;
                this->height = height;
                this->width = width;
                dim.resize(2);
                dim[0] = height;
                dim[1] = width;
            }
            int getChannelCount() {
                return 3;
            }
            std::vector<int> getDim() {
                return dim;
            }
            unsigned char getData(int channel,const std::vector<int>& position) {
                return f(channel, position[0], position[1]);
            }
            bool isKeyFrame() {
                return true;
            }
    };

    template<typename Func>
    dtcode::data::FramePtr F(int height, int width, const Func& f) {
        return dtcode::data::FramePtr(new FrameF<Func>(height, width, f));
    }
}