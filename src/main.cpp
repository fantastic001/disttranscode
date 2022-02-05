
// #include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ffmpeg/FFMpegVideoWriter.hpp>
#include <frame/FrameF.hpp>

using namespace std; 
using namespace dtcode::ffmpeg;
using namespace dtcode::frame;
using namespace dtcode::data;

int main(int argc, char** argv) {
    // MPI::Init(argc, argv);

    FFMpegVideoWriter writer(argv[1], argv[2]);
    writer.writeFrames([] (int num) {
        return F(10, 10, [num] (int c,int y,int x) {
            double r = num / 255.0;
            return rgb2yuv(1,r,1.0, c);
        });
    }, 255);
    // MPI::Finalize();
}


