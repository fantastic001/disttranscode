
// #include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ffmpeg/FFMpegVideoWriter.hpp>


using namespace std; 
using namespace dtcode::ffmpeg;

int main(int argc, char** argv) {
    // MPI::Init(argc, argv);

    FFMpegVideoWriter writer(argv[1], argv[2]);
    for (int i = 0; i<250; i++) writer.writeFrame(dtcode::data::FramePtr());
    // MPI::Finalize();
}


