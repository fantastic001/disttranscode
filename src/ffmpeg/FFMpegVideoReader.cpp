
#include <ffmpeg/FFMpegVideoReader.hpp>
#include <ffmpeg/utils.hpp>

using namespace dtcode::ffmpeg;
namespace utils = dtcode::ffmpeg::utils;
using namespace dtcode::data;
using namespace std;

FFMpegVideoReader::FFMpegVideoReader(string path) : path(path) {

}

StreamPtr FFMpegVideoReader::getVideoStream() {
    auto streams = utils::get_stream_from_file(path);
    if (streams.size() == 0) return nullptr;
    else return streams.front();
}

StreamPtr FFMpegVideoReader::getAudioStream() 
{
    return nullptr;
}
