
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
    auto segments_list = distribution->distribute(stream);
    if (segments_list.size() == 0 && distribution->nextIndex() == -1) 
        return vector<SegmentPtr>();
    vector<SegmentPtr> segments;
    for (auto seg : segments_list) {
        segments.push_back(seg);
    }
    cout << "On one node number of segments is " << segments.size() << endl;
    int index;
    map<int, SegmentPtr> index_segment_map;
    while ((index = distribution->nextIndex()) >= 0) {
        auto segment = distribution->getSegment(index);
        cout << "Segment index " << index << endl;
        optional<FramePtr> frame; 
        auto encoder = make_shared<FFMpegVideoEncoder>();
        while ((frame = segment->nextFrame()).has_value()) {
            auto ff = frame.value();
            for (auto filter : filters) {
                ff = filter->filter(ff);
            }
            encoder->writeFrame(ff);
        }
        cout << "Writing transformed segment on index " << index << endl;
        index_segment_map[index] = encoder->getSegment();
    
    }
    segments.clear();
    bool finalized = false;
    while (!finalized) {
        auto consensus = consensusFactory->create();
        int proposal;
        if (index_segment_map.empty()) proposal = -1;
        else proposal = index_segment_map.begin()->first;
        cout << "Sending proposal for index " << proposal << endl;
        consensus->propose(proposal);
        int decision = consensus->getDecision();
        cout << "Decision is " << decision << endl;
        if (decision < 0 ) finalized = true;
        else {
            SegmentPtr seg;
            cout << (root ? "on root " : "worker ") << " " << decision << " proposal=" << proposal <<endl;
            cout.flush();
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