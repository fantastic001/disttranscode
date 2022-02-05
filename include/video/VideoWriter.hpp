
#pragma once 

#include <data/Frame.hpp>
#include <frame/FrameGenerator.hpp>
namespace dtcode::video {
    class VideoWriter {

        public:
            virtual void writeFrame(dtcode::data::FramePtr frame) =0;
            
            template <typename G>
            void writeFrames(G generator, int count) {
                for (int i = 0; i<count; i++) writeFrame(generator(i));
            }
    };
}