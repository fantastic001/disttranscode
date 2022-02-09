
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ffmpeg/FFMpegVideoWriter.hpp>
#include <ffmpeg/FFMpegVideoStream.hpp>
#include <frame/FrameF.hpp>

#include <data/FilterManager.hpp>
#include <cmd/ArgumentParser.hpp>

#include <net/NodeContext.hpp>

using namespace std; 
using namespace dtcode::ffmpeg;
using namespace dtcode::frame;
using namespace dtcode::data;
using namespace dtcode::cmd;
using namespace dtcode::net;

int main(int argc, char** argv) {
    NodeContext context(&argc, &argv);
    ArgumentParser parser(argc, argv);
    cout << "My rank " << context.rank();

    if (context.rank() == 0) {
        for (int i = 1; i<context.size(); i++) {
            context[i] << 42;
        }
    }
    else {
        int magic; 
        context[0] >> magic;
        cout << magic << " on rank " << context.rank() << endl;
    }

    cout << "Input: " << parser.getInputLocation() << endl 
        << "Output: " << parser.getOutputLocation() << endl;
    
    {
        FFMpegVideoWriter writer(parser.getOutputLocation(), "mpeg4");

        // string borat = "/home/stefan//Downloads/Borat (2006) [1080p]/Borat.2006.1080p.BrRip.x264.YIFY.mp4";
        // FFMpegVideoStream stream(borat);
        FFMpegVideoStream stream(parser.getInputLocation());
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

    auto filter_names = FilterManager::getInstance()->getAllFilterNames();
    for (auto name : filter_names) {
        cout << "Filter: " << name << endl;
    }

    // FFMpegVideoWriter writer2("a.mp4", "mpeg4");
    // writer2.writeFrames([] (int num) {
    //     return F(10, 10, [num] (int c,int y,int x) {
    //         if (num < 12) return rgb2yuv(0,0,0.0, c);
    //         else return rgb2yuv(1,1,1,c);
    //     });
    // }, 25);

}


