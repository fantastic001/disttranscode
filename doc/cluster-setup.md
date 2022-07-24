
import modules 

    module load openmpi/4.0.5 

export path 

    export PKG_CONFIG_PATH=/home/student/stefan_nozinic/pkgconfig

    export PATH="/home/student/stefan_nozinic/3rdparty/gcc/bin:/opt/openmpi-4.0.5/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin:/usr/local/lib64/python3.6/site-packages/pycompss/COMPSs/Runtime/scripts/user:/usr/local/lib64/python3.6/site-packages/pycompss/COMPSs/Runtime/scripts/utils:/usr/local/lib64/python3.6/site-packages/pycompss/COMPSs/Bindings/c/bin:/home/student/.local/bin:/home/student/bin:/usr/local/lib64/python3.6/site-packages/pycompss/COMPSs/Runtime/scripts/user:/usr/local/lib64/python3.6/site-packages/pycompss/COMPSs/Runtime/scripts/utils:/usr/local/lib64/python3.6/site-packages/pycompss/COMPSs/Bindings/c/bin:/home/student/.local/bin:/home/student/bin"


First, remove testDisttranscode target in cmakelists 

run cmake

    export CC=$(which gcc)
    export CXX=$(which g++)
    ../3rdparty/cmake/usr/local/bin/cmake  .

    cp -r ../include/* include/
    export CFLAGS="-I /home/student/stefan_nozinic/3rdparty/ffmpeg/include/"
    export CXXFLAGS="-I /home/student/stefan_nozinic/3rdparty/ffmpeg/include/"
    export LDFLAGS="-L /home/student/stefan_nozinic/3rdparty/ffmpeg/lib"
    make
    export LD_LIBRARY_PATH="/home/student/stefan_nozinic/3rdparty/ffmpeg/lib/:/home/student/stefan_nozinic/3rdparty/gcc/lib64/"

running the job 

    cd ~/stefan_nozinic/disttranscode
    sbatch --nodes 1 test.sh

Measurements of the job 

    sacct -l -j jobid 

# Measurements 

The following table shows some measurements of performance for sample video 

| Number of nodes | Time         |
|-----------------|--------------|
| 1               | 00:11:41     |
| 2               | 00:11:44     |
| 3               | 00:07:45     |
| 4               | 00:05:47     |