cmake_minimum_required (VERSION 3.17)

project (DistTranscode)

find_package(Threads REQUIRED)


enable_testing()
find_package(MPI REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice
    libavfilter
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)



set(SOURCES 
src/cmd/ArgumentParser.cpp
src/data/FilterManager.cpp
src/data/Frame.cpp
src/data/Segment.cpp
src/ffmpeg/FFMpegVideoEncoder.cpp
src/ffmpeg/FFMpegVideoFrame.cpp
src/ffmpeg/FFMpegVideoReader.cpp
src/ffmpeg/FFMpegVideoSegment.cpp
src/ffmpeg/FFMpegVideoStream.cpp
src/ffmpeg/FFMpegVideoWriter.cpp
src/filters/black/BlackFilter.cpp
src/filters/grayscale/GrayscaleFilter.cpp
src/main.cpp
src/net/NodeComm.cpp
src/net/NodeContext.cpp

)

add_executable(disttranscode ${SOURCES})

if(${MPI_FOUND})
    target_link_libraries(disttranscode PUBLIC MPI::MPI_CXX)
endif(${MPI_FOUND})

target_link_libraries(disttranscode PRIVATE PkgConfig::LIBAV )

target_include_directories(disttranscode PUBLIC include/)
set(CMAKE_BUILD_TYPE Debug)
