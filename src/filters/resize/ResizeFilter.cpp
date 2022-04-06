
#include <data/FilterManager.hpp>
#include <frame/BilinearInterpolation.hpp>

#include <frame/FrameF.hpp>

using namespace std; 
using namespace dtcode::data; 
using namespace dtcode::frame;

class ResizeFilter : public Filter {
    public:
        int height, width; 
        ResizeFilter(int height, int width) : height(height), width(width) {

        }
        FramePtr filter(FramePtr frame, int frameNumber) {
            return F(height, width, 
                [frame, this] (int channel, int y, int x) {
                    auto interp = make_shared<BilinearInterpolation>(frame);
                    return interp->at(channel, x / (1.0*this->width), y / (1.0 * this->height));
            });
        }
};

class ResizeFilterFactory : public FilterFactory {
    public:
        FilterPtr create() {
            return make_shared<ResizeFilter>(getParameter("height", 100), getParameter("width", 100));
        }
};


class ResizeFilterParser : public FilterArgumentParser {
    public:
        FilterFactoryPtr parse(vector<string>::iterator start, vector<string>::iterator end) {
            FilterFactoryPtr factory = make_shared<ResizeFilterFactory>();
            factory->setParameter("width", *start);
            start++;
            factory->setParameter("height", *start);
            return factory;
        }
};

REGISTER_FILTER(resize, ResizeFilterParser)