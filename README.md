
# Usage 

Usage:

    disttranscode --input location --filter f1 param1 param2 --filter f2 param1 param2 output-location

filters have its name and parameter and they are applied in order they are specified 

* id - copy video 
* grayscale - convert video to grayscale 
* resize newwidth newheight - resize video 
* watermark x y text fontsize color startFrame endFrame - put watermark on video
* image x y image-location startFrame endFrame - put image on whole video 

output can be:

* file location 
* chromecast 
* video played on master node 


For instance, to resize video a.mp4 to 1000x1000 and distribute that work on two nodes and write result as b.mp4:

    mpirun -n 2 ./disttranscode -i a.mp4 -f resize 1920 1080  b.mp4

# Dependencies

* libav
* openmpi