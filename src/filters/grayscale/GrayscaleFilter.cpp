
#include <data/FilterManager.hpp>

#include <frame/FrameF.hpp>

using namespace std; 
using namespace dtcode::data; 
using namespace dtcode::frame;

class GrayscaleFilter : public Filter {
    public:
        FramePtr filter(FramePtr frame) {
            return F(frame->getDim()[0], frame->getDim()[1], 
                [frame] (int channel, int y, int x) {
                    return (channel == 0) * frame->getData(0, pos2d(y,x))
                        + (channel == 1) * 127
                        + (channel == 2) * 127
                    ;
            });
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