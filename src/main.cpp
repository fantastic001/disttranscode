
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
    cout << "Network size: " << context.size() << endl;
    cout << "Input: " << parser.getInputLocation() << endl 
        << "Output: " << parser.getOutputLocation() << endl;
    
    auto filter_names = FilterManager::getInstance()->getAllFilterNames();
    for (auto name : filter_names) {
        cout << "Filter: " << name << endl;
    }
    auto filters = parser.getFilters();

    FFMpegVideoWriter writer(parser.getOutputLocation(), "mpeg4");

    FFMpegVideoStream stream(parser.getInputLocation());
    auto segments = stream.parse();
    if (context.rank() == 0) cout << "Number of segments: " << segments.size() << endl;
    int i = 0;
    for (auto segment : segments) {
        i++;
        if ((i-1) % context.size() == context.rank()) {
            cout << "Segment: " << i << " on rank " << context.rank() << endl;
            auto frames = segment->decodeAllFrames();
            cout << "Number of frames: " << frames.size() << " on rank " << context.rank() << endl;
            for (auto frame : frames) {
                cout << endl << frame->getDim()[0] << " " << frame->getDim()[1] << endl;
                for (auto filter : filters) {
                    frame = filter->filter(frame);
                }
                writer.writeFrame(frame);
            }
        } 
    }


    // FFMpegVideoWriter writer2("a.mp4", "mpeg4");
    // writer2.writeFrames([] (int num) {
    //     return F(10, 10, [num] (int c,int y,int x) {
    //         if (num < 12) return rgb2yuv(0,0,0.0, c);
    //         else return rgb2yuv(1,0,0,c);
    //     });
    // }, 25);

}


