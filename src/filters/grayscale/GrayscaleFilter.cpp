
#include <data/FilterManager.hpp>

#include <frame/FrameF.hpp>

using namespace std; 
using namespace dtcode::data; 
using namespace dtcode::frame;

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
            if (first_frame) {
                dim = frame->getDim();
                first_frame = false;
            }
            return F(dim[0], dim[1], 
                [frame, this] (int channel, int y, int x) {
                    position[0] = y;
                    position[1] = x;
                    return (channel == 0) * frame->getData(0, position)
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