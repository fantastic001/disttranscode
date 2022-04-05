
#include <data/Segment.hpp>

using namespace std;
using namespace dtcode::data;

list<shared_ptr<Frame>> Segment::decodeAllFrames() {
    list<shared_ptr<Frame>> result; 
    while (true) {
        auto frame = nextFrame();
        if (frame.has_value()) {
            result.push_back(frame.value());
        }
        else return result;
    }
}

int Segment::getFrameCount() {
    return decodeAllFrames().size();
}