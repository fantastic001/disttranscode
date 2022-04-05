#pragma once 

#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <utility>
#include "gmock/gmock.h"  // Brings in Google Mock.

#include <data/Stream.hpp>
#include <net/Distribution.hpp>
#include <net/Consensus.hpp>
#include <net/ConsensusFactory.hpp>
#include <net/DistributionFactory.hpp>
#include <frame/FrameF.hpp>
#include <data/Filter.hpp>

using namespace dtcode::data;
using namespace std; 
using namespace testing;

class MockStream : public Stream {
 public:
  MOCK_METHOD0(parse, list<SegmentPtr>());
  MOCK_METHOD0(getData, std::vector<uint8_t>());
};

class MockSegment : public Segment {
 public:
  MOCK_METHOD0(nextFrame, optional<FramePtr>());
  MOCK_METHOD0(containsKeyFrame, bool());
  MOCK_METHOD0(decodeKeyFrames, list<FramePtr>());
  MOCK_METHOD0(serialize, vector<uint8_t>());
};


class MockFrame : public Frame {
 public:
  MOCK_METHOD3(getData, uint8_t(int, int, int));
  MOCK_METHOD0(getDim, std::vector<int>());
  MOCK_METHOD0(getChannelCount, int());
  MOCK_METHOD0(isKeyFrame, bool());
};
class MockDistribution : public dtcode::net::Distribution {
 public:
  MOCK_METHOD1(distribute, StreamPtr(StreamPtr));
  MOCK_METHOD0(nextIndex, int());
  MOCK_METHOD1(getRank, int(int));
  MOCK_METHOD2(getSegment, SegmentPtr(int, SegmentPtr));
  MOCK_METHOD1(getFrameCountBeforeThisSegment, int(int));
};
class MockConsensus : public dtcode::net::Consensus {
 public:
  MOCK_METHOD1(propose, void(int));
  MOCK_METHOD0(getDecision, int());
};

class MockConsensusFactory : public dtcode::net::ConsensusFactory {
  public:
    MOCK_METHOD0(create, std::shared_ptr<::dtcode::net::Consensus>()); 
};


class MockDistributionFactory : public dtcode::net::DistributionFactory {
  public:
    MOCK_METHOD0(create, std::shared_ptr<::dtcode::net::Distribution>()); 
};

class MockFilter : public Filter {
  public:
    MOCK_METHOD1(filter, FramePtr(FramePtr));
};