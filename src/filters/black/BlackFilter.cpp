
#include <data/FilterManager.hpp>

#include <frame/FrameF.hpp>

using namespace std; 
using namespace dtcode::data; 
using namespace dtcode::frame;

class BlackFilter : public Filter {
        int height, width; 
    public:
        BlackFilter(int height, int width) : height(height), width(width) {

        }
        FramePtr filter(FramePtr frame, int frameNumber) {
            return F(height, width, 
                [] (int channel, int y, int x) {
                    return rgb2yuv(0.0, 0.0, 0.0, channel);
            });
        }
};

class BlackFilterFactory : public FilterFactory {
    public:
        FilterPtr create() {
            return make_shared<BlackFilter>(getParameter("height", 100), getParameter("width", 100));
        }
};


class BlackFilterParser : public FilterArgumentParser {
    public:
        FilterFactoryPtr parse(vector<string>::iterator start, vector<string>::iterator end) {
            FilterFactoryPtr factory = make_shared<BlackFilterFactory>();
            factory->setParameter("width", *start);
            start++;
            factory->setParameter("height", *start);
            return factory;
        }
};

REGISTER_FILTER(black, BlackFilterParser)