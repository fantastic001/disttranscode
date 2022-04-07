
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ffmpeg/FFMpegVideoWriter.hpp>
#include <ffmpeg/FFMpegVideoEncoder.hpp>
#include <ffmpeg/FFMpegVideoStream.hpp>
#include <ffmpeg/FFMpegVideoReader.hpp>
#include <frame/FrameF.hpp>

#include <data/FilterManager.hpp>
#include <cmd/ArgumentParser.hpp>

#include <net/NodeContext.hpp>

#include <net/Synchronizer.hpp>
#include <net/UniformFloodingConsensus.hpp>
#include <net/StrideDistribution.hpp>

using namespace std; 
using namespace dtcode::ffmpeg;
using namespace dtcode::frame;
using namespace dtcode::data;
using namespace dtcode::cmd;
using namespace dtcode::net;


class MyConsensusFactory : public ConsensusFactory {
        shared_ptr<NodeContext> context;
    public:
        MyConsensusFactory(shared_ptr<NodeContext> context) : context(context) {

        }
        ConsensusPtr create() {
            return make_shared<UniformFloodingConsensus<NodeContext>>(context);
        }
};


class MyDistributionFactory : public DistributionFactory {
        shared_ptr<NodeContext> context;
    public:
        MyDistributionFactory(shared_ptr<NodeContext> context) : context(context) {

        }
        DistributionPtr create() {
            return make_shared<StrideDistribution<NodeContext>>(context);
        }
};

int main(int argc, char** argv) {
    auto context = make_shared<NodeContext>(&argc, &argv);
    ArgumentParser parser(argc, argv);
    cout << "Network size: " << context->size() << endl;
    cout << "Input: " << parser.getInputLocation() << endl 
        << "Output: " << parser.getOutputLocation() << endl;
    
    auto filter_names = FilterManager::getInstance()->getAllFilterNames();
    for (auto name : filter_names) {
        cout << "Filter: " << name << endl;
    }
    auto filters = parser.getFilters();

    auto consensusFactory = make_shared<MyConsensusFactory>(context);
    auto distributionFactory = make_shared<MyDistributionFactory>(context);
    cout << "Rank " << context->rank() << endl;
    auto sync = make_shared<Synchronizer>(distributionFactory, consensusFactory, context->rank() == 0);
    for (auto filter : filters) {
        sync->addFilter(filter);
    }
    auto reader = make_shared<FFMpegVideoReader>(parser.getInputLocation());
    auto stream = reader->getVideoStream();
    if (stream == nullptr) {
        cerr << "Error while opning file\n";
        return 1;
    }
    auto segments = sync->process(stream);
    cout << "Node: " << context->rank() << " finished processing\n";
    if (context->rank() == 0) {
        FFMpegVideoWriter writer(parser.getOutputLocation(), "mpeg4");
        for (auto seg : segments) {
            writer.writeSegment(seg);
        }
    }
    return 0;
}


