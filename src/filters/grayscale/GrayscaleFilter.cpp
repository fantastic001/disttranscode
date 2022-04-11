
#include <data/FilterManager.hpp>


using namespace std; 
using namespace dtcode::data; 

class GrayscaleFrame : public Frame {
        FramePtr source; 
        std::vector<int> dim;
    public:
        GrayscaleFrame(FramePtr source) : source(source) {
            dim = source->getDim();
        }
        int getChannelCount() {
            return 3;
        }
        std::vector<int> getDim() {
            return dim;
        }
        unsigned char getData(int channel,const std::vector<int>& position) {
            return (channel == 0) * source->getData(0, position)
                        + (channel == 1) * 127
                        + (channel == 2) * 127
                    ;
        }
        bool isKeyFrame() {
            return source->isKeyFrame();
        }
};

class GrayscaleFilter : public Filter {
        bool first_frame;
        vector<int> dim;
        vector<int> position;
    public:
        GrayscaleFilter() {
            first_frame = true;
            position.resize(2);
        }
        FramePtr filter(FramePtr frame, int frameNumber) {
            return make_shared<GrayscaleFrame>(frame);
        }
};

class GrayscaleFilterFactory : public FilterFactory {
    public:
        FilterPtr create() {
            return make_shared<GrayscaleFilter>();
        }
};


class GrayscaleFilterParser : public FilterArgumentParser {
    public:
        FilterFactoryPtr parse(vector<string>::iterator start, vector<string>::iterator end) {
            FilterFactoryPtr factory = make_shared<GrayscaleFilterFactory>();
            return factory;
        }
};

REGISTER_FILTER(grayscale, GrayscaleFilterParser)