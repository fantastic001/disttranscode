
# Usage 

Usage:

    disttranscode --input location --filter f1 param1 param2 --filter f2 param1 param2 output-location

input location and output location are locations visible to all machines. This is usually shared network drive. 

filters have its name and parameter and they are applied in order they are specified 

* id width height - copy video 
* grayscale width height - convert video to grayscale 
* resize width height newwidth newheight - resize video 
* watermark width height x y text fontsize color - put watermark on video
* image width height x y image-location - put image on whole video 

output can be:

* file location 
* chromecast 
* video played on master node 

# Dependencies

* libav
* openmpi