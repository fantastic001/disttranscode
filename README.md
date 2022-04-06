
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

# Dependencies

* libav
* openmpi