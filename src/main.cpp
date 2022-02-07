
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

        // string borat = "/home/stefan//Downloads/Borat (2006) [1080p]/Borat.2006.1080p.BrRip.x264.YIFY.mp4";
        // FFMpegVideoStream stream(borat);
        FFMpegVideoStream stream("a.mp4");
        cout << "Parsing segments\n";
        auto segments = stream.parse();
        cout << "Number of segments: " << segments.size() << endl;
        int i = 0;
        for (auto segment : segments) {
            i++;
            cout << "Segment: " << i << " ";
            if (segment->containsKeyFrame()) cout << " with key frame\n";
            else cout << endl;
            if (i == 2) {
                segment->decodeKeyFrames();
                continue;
            }
            if (i < 2) continue;
            cout << "Decoding segment " << i << endl;
            auto frames = segment->decodeAllFrames();
            cout << "Number of frames: " << frames.size() << endl;
            for (auto frame : frames) {
                cout << endl << frame->getDim()[0] << " " << frame->getDim()[1] << endl;
                writer.writeFrame(F(frame->getDim()[0], frame->getDim()[1], [frame] (int channel, int y, int x) {
                    auto pixel = frame->getData(channel, pos2d(y,x));
                    // cout << (int)pixel << " ";
                    if (frame->isKeyFrame()) {
                        // cout << "Keyframe found\n";
                    }
                    return pixel;
                }));
            }
        }
    }
    // FFMpegVideoWriter writer2("a.mp4", "mpeg4");
    // writer2.writeFrames([] (int num) {
    //     return F(10, 10, [num] (int c,int y,int x) {
    //         if (num < 12) return rgb2yuv(0,0,0.0, c);
    //         else return rgb2yuv(1,1,1,c);
    //     });
    // }, 25);


    // MPI::Finalize();
}


