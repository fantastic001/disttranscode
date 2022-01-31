cmake_minimum_required (VERSION 3.17)

project (DistTranscode)

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
    src/main.cpp
)

add_executable(disttranscode ${SOURCES})

if(${MPI_FOUND})
    target_link_libraries(disttranscode PUBLIC MPI::MPI_CXX)
endif(${MPI_FOUND})

target_link_libraries(disttranscode PRIVATE PkgConfig::LIBAV )
