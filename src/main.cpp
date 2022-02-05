
// #include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ffmpeg/FFMpegVideoWriter.hpp>
#include <ffmpeg/FFMpegVideoStream.hpp>
#include <frame/FrameF.hpp>

using namespace std; 
using namespace dtcode::ffmpeg;
using namespace dtcode::frame;
using namespace dtcode::data;

int main(int argc, char** argv) {
    // MPI::Init(argc, argv);
    {
        FFMpegVideoWriter writer("b.mp4", "mpeg4");

        FFMpegVideoStream stream("a.mp4");
        cout << "Parsing segments\n";
        auto segments = stream.parse();
        for (auto segment : segments) {
            // cout << "Segment decoded\n";
            // cout << "Decoding frames in a given segment\n";
            cout.flush();
            auto frames = segment->decodeAllFrames();
            for (auto frame : frames) {
                // cout << endl << frame->getDim()[0] << " " << frame->getDim()[1] << endl;
                writer.writeFrame(F(frame->getDim()[0], frame->getDim()[1], [frame] (int channel, int y, int x) {
                    auto pixel = frame->getData(channel, pos2d(y,x));
                    cout << pixel << " ";
                    return pixel;
                }));
            }
        }
    }
    FFMpegVideoWriter writer2("a.mp4", "mpeg4");
    writer2.writeFrames([] (int num) {
        return F(10, 10, [num] (int c,int y,int x) {
            double r = num / 255.0;
            return rgb2yuv(1,r,1.0, c);
        });
    }, 255);


    // MPI::Finalize();
}


