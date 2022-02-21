
#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <utility>
#include "gmock/gmock.h"  // Brings in Google Mock.

#include <data/Stream.hpp>

using namespace dtcode::data;
using namespace std; 
using namespace testing;

class MockStream : public Stream {
 public:
  MOCK_METHOD0(parse, list<SegmentPtr>());
};

class MockSegment : public Segment {
 public:
  MOCK_METHOD0(nextFrame, optional<FramePtr>());
  MOCK_METHOD0(containsKeyFrame, bool());
  MOCK_METHOD0(decodeKeyFrames, list<FramePtr>());
  MOCK_METHOD0(serialize, vector<uint8_t>());
};
