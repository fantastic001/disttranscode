
#include <frame/BilinearInterpolation.hpp>

using namespace dtcode::data; 
using namespace dtcode::frame;


int floor(double x) {
    if (x - (int)x < 0.9999999999) {
        return (int) x;
    }
    else {
        return (int) x + 1;
    }
}

BilinearInterpolation::BilinearInterpolation(FramePtr frame)  : frame(frame) {

}

uint8_t BilinearInterpolation::at(int channel, double x, double y) {
    double a00, a01, a10, a11;
    int height = frame->getDim()[0];
    int width = frame->getDim()[1];
    auto coef = width * height;
    int j = floor(width * x);
    int i = floor(height * y);
    double x1 = j / (width * 1.0);
    double x2 = (j+1) / (width * 1.0);
    double y1 = i / (height * 1.0);
    double y2 = (i+1) / (height * 1.0);
    a00 = (x - x2) * (y - y2) * coef;
    a01 = -(x - x2) * (y - y1) * coef;
    a10 = -(x - x1) * (y - y2) * coef;
    a11 = (x - x1) * (y - y1) * coef;

    return a00 * frame->getData(channel, pos2d(i,j))
        + a01 * frame->getData(channel, pos2d(i+1,j))
        + a10 * frame->getData(channel, pos2d(i,j+1))
        + a11 * frame->getData(channel, pos2d(i+1,j+1));
}