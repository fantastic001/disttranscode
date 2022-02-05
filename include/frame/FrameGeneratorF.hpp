#pragma once


#include<frame/FrameGenerator.hpp>
#include <functional>

namespace dtcode::frame {
    template<typename F>
    class FrameGeneratorF : public FrameGenerator {
            std::function<dtcode::data::FramePtr (int)> f;
        public:
            FrameGeneratorF(F f) {
                this->f = f;
            }
            dtcode::data::FramePtr generateFrame(int frameNumber) {
                return f(frameNumber);
            }

    };
    
    template<typename F>
    FrameGenerator&& G(const F& f) {
        return FrameGeneratorF(f);
    }
}