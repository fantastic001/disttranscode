

#include <ffmpeg/FFMpegVideoWriter.hpp>
#include <frame/FrameF.hpp>
#include <iostream>

using namespace dtcode::ffmpeg;
using namespace dtcode::frame;
using namespace dtcode::data;
using namespace std; 

#define FRAMERATE 25
#define DURATION_SEC 6

int main() {
    FFMpegVideoWriter writer("test.mp4", "mpeg4");
    int size = FRAMERATE * DURATION_SEC;
    cout << "Starting to generate frames\n";
    for (int i = 0; i<size; i++) {
        cout << "   "<< 100 * i / size << "% Generated\r";
        cout.flush();
        writer.writeFrame(F(1080, 1920, [i] (int channel, int x, int y) {
            return rgb2yuv(i < FRAMERATE * DURATION_SEC / 2  ? 1 : 0, 0, 0, channel);
        }));
    }
    cout << endl;
    return 0;
}