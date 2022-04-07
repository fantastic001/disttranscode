
#include <net/Synchronizer.hpp>
#include <frame/FrameF.hpp>
#include <ffmpeg/FFMpegVideoEncoder.hpp>
#include <map>

#include <iostream>


using namespace dtcode::net; 
using namespace dtcode::data;
using namespace dtcode::ffmpeg;
using namespace dtcode::frame;
using namespace std;

Synchronizer::Synchronizer(shared_ptr<DistributionFactory> distributionFactory, shared_ptr<ConsensusFactory> consensusFactory, bool root) 
: consensusFactory(consensusFactory), distributionFactory(distributionFactory), root(root) {

}

vector<SegmentPtr> Synchronizer::process(StreamPtr stream) {
    auto distribution = distributionFactory->create();
    stream = distribution->distribute(stream);
    int size = stream->parse().size();
    int index;
    map<int, SegmentPtr> index_segment_map;
    auto consensus = consensusFactory->create();
    while ((index = distribution->nextIndex()) >= 0) {
        auto segment = distribution->getSegment(index);
        cout 
            << 100 * index / size 
            << "% Segment index " << index 
            << " after " << distribution->getFrameCountBeforeThisSegment(index)<< " frames\r";
        optional<FramePtr> frame; 
        auto encoder = make_shared<FFMpegVideoEncoder>();
        int frame_counter = 0;
        while ((frame = segment->nextFrame()).has_value()) {
            auto ff = frame.value();
            for (auto filter : filters) {
                ff = filter->filter(ff, distribution->getFrameCountBeforeThisSegment(index) + frame_counter);
            }
            frame_counter++;
            encoder->writeFrame(ff);
            frame_counter++;
        }
        index_segment_map[index] = encoder->getSegment();
    }
    cout << endl;
    std::vector<dtcode::data::SegmentPtr> segments;
    bool finalized = false;
    while (!finalized) {
        int proposal;
        if (index_segment_map.empty()) proposal = -1;
        else proposal = index_segment_map.begin()->first;
        consensus->propose(proposal);
        int decision = consensus->getDecision();
        if (decision >= size) finalized = true;
        else {
            SegmentPtr seg;
            if (decision == proposal) {
                seg = distribution->getSegment(proposal, (index_segment_map.begin()->second));
                index_segment_map.erase(index_segment_map.begin());
                segments.push_back(seg);
            }
            else {
                seg = distribution->getSegment(decision);
                segments.push_back(seg);
            }
            // segments[decision] = seg;
        }    
    }
    return segments;
}

void Synchronizer::addFilter(FilterPtr filter) {
    filters.push_back(filter);
}