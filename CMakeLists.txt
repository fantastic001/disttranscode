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

    src/net/NodeComm.cpp
    src/net/NodeContext.cpp
    src/net/Synchronizer.cpp

)

set(TEST_SOURCES
    test/main.cpp
    test/mock_net.cpp
    test/TestUniformFloodingConsensus.cpp
    test/TestStrideDistribution.cpp
    test/TestSynchronizer.cpp

)

add_executable(disttranscode ${SOURCES} src/main.cpp)

add_executable(testDisttranscode 
    ${SOURCES}
    ${TEST_SOURCES}
)

if(${MPI_FOUND})
    target_link_libraries(disttranscode PUBLIC MPI::MPI_CXX)
    target_link_libraries(testDisttranscode PUBLIC MPI::MPI_CXX)
endif(${MPI_FOUND})

target_link_libraries(disttranscode PUBLIC PkgConfig::LIBAV )
target_link_libraries(testDisttranscode PUBLIC PkgConfig::LIBAV )

include_directories(PUBLIC include/)
set(CMAKE_BUILD_TYPE Debug)


target_link_libraries(testDisttranscode PUBLIC gtest gmock)
add_test(NAME test_disttranscode COMMAND testDisttranscode)