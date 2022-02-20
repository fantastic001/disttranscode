
#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <utility>
#include <net/StrideDistribution.hpp>
#include "gmock/gmock.h"  // Brings in Google Mock.

using namespace dtcode::net;
using namespace dtcode::data;
using namespace std; 
using namespace testing;


struct Network {
    int size; 
    int rank;
    vector<vector<uint8_t>> messages;
    vector<int> senders;
    vector<int> receivers; 
};

struct NetworkCommFake {
    shared_ptr<Network> net;
    int from, to;
    NetworkCommFake(shared_ptr<Network> net, int from, int to)  : net(net), from(from), to(to) {

    }

    NetworkCommFake& operator<<(vector<uint8_t> data) {
        net->messages.push_back(data);
        net->senders.push_back(from);
        net->receivers.push_back(to);
        return *this;
    }

    NetworkCommFake& operator>>(vector<uint8_t>& data) {
        for (int i = net->messages.size()-1; i>=0; i--) {
            if (net->senders[i] == from && (net->receivers[i] == to || net->receivers[i] == net->size)) {
                data = net->messages[i];
            }
        }
        return *this;
    }
};

struct NetworkFake
{
    shared_ptr<Network> net;

    NetworkFake(shared_ptr<Network> net) : net(net) {
    }

    NetworkFake& operator<<(const vector<uint8_t>& data) {
        net->messages.push_back(data);
        net->senders.push_back(net->rank);
        net->receivers.push_back(net->size);
    }

    NetworkCommFake operator[] (int to) {
        return NetworkCommFake(net, net->rank, to);
    }

    int size() {
        return net->size;
    }
    int rank() {
        return net->rank;
    }
};



class StrideDistributionTest : public ::testing::Test {
 protected:
    void SetUp() override {
            net = make_shared<Network>();
            net->size = 3;
            net->rank = 0;
            context = make_shared<NetworkFake>(net);
            distribution = make_shared<StrideDistribution<NetworkFake>>(context);
    }
    shared_ptr<Network> net;
    shared_ptr<NetworkFake> context;
    DistributionPtr distribution;
};


TEST(TestStrideDistributionTestBasic, test_creation) {
    auto context = make_shared<NetworkFake>(make_shared<Network>());
    DistributionPtr distribution = make_shared<StrideDistribution<NetworkFake>>(context);
}

TEST_F(StrideDistributionTest, distribute_will_not_send_message_on_null) {
    distribution->distribute(nullptr);
    ASSERT_EQ(net->messages.size(), 0);
}

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


TEST_F(StrideDistributionTest, distribute_numbver_of_messages_equals_number_of_segments) {
    auto segment = make_shared<MockSegment>();
    EXPECT_CALL(*segment, serialize()).WillOnce(Return(vector<uint8_t> {1,2,3}));
    auto stream = make_shared<MockStream>();
    EXPECT_CALL(*stream, parse()).WillOnce(Return(list<SegmentPtr> {segment}));
    distribution->distribute(stream);
    ASSERT_EQ(net->messages.size(), 0);
}

TEST_F(StrideDistributionTest, distribute_onebyone) {
    auto segment0 = make_shared<MockSegment>();
    auto segment1 = make_shared<MockSegment>();
    auto segment2 = make_shared<MockSegment>();
    auto segment3 = make_shared<MockSegment>();
    EXPECT_CALL(*segment0, serialize()).WillRepeatedly(Return(vector<uint8_t> {1,2,3}));
    EXPECT_CALL(*segment1, serialize()).WillRepeatedly(Return(vector<uint8_t> {2,2,3}));
    EXPECT_CALL(*segment2, serialize()).WillRepeatedly(Return(vector<uint8_t> {3,2,3}));
    EXPECT_CALL(*segment3, serialize()).WillRepeatedly(Return(vector<uint8_t> {4,2,3}));
    auto stream = make_shared<MockStream>();
    EXPECT_CALL(*stream, parse()).WillOnce(Return(list<SegmentPtr> {segment0, segment1, segment2, segment3}));
    distribution->distribute(stream);
    ASSERT_EQ(net->messages.size(), 4);
    EXPECT_EQ(net->receivers[0], 1);
    EXPECT_EQ(net->receivers[1], 1);
    EXPECT_EQ(net->receivers[2], 2);
    EXPECT_EQ(net->receivers[3], 2);
    EXPECT_EQ(net->senders[3], 0);
}

TEST_F(StrideDistributionTest, next_index_should_return_negative_one_if_no_remaining_segments) {
    auto segment = make_shared<MockSegment>();
    EXPECT_CALL(*segment, serialize()).WillRepeatedly(Return(vector<uint8_t> {1,2,3}));
    auto stream = make_shared<MockStream>();
    EXPECT_CALL(*stream, parse()).WillOnce(Return(list<SegmentPtr> {segment}));
    distribution->distribute(stream);
    ASSERT_EQ(distribution->nextIndex(), 0);
    ASSERT_EQ(distribution->nextIndex(), -1);
}

TEST_F(StrideDistributionTest, distribute_getrank) {
    auto segment0 = make_shared<MockSegment>();
    auto segment1 = make_shared<MockSegment>();
    auto segment2 = make_shared<MockSegment>();
    auto segment3 = make_shared<MockSegment>();
    EXPECT_CALL(*segment0, serialize()).WillRepeatedly(Return(vector<uint8_t> {1,2,3}));
    EXPECT_CALL(*segment1, serialize()).WillRepeatedly(Return(vector<uint8_t> {2,2,3}));
    EXPECT_CALL(*segment2, serialize()).WillRepeatedly(Return(vector<uint8_t> {3,2,3}));
    EXPECT_CALL(*segment3, serialize()).WillRepeatedly(Return(vector<uint8_t> {4,2,3}));
    auto stream = make_shared<MockStream>();
    EXPECT_CALL(*stream, parse()).WillOnce(Return(list<SegmentPtr> {segment0, segment1, segment2, segment3}));
    distribution->distribute(stream);
    ASSERT_EQ(distribution->getRank(0), 0);
    ASSERT_EQ(distribution->getRank(1), 1);
    ASSERT_EQ(distribution->getRank(2), 2);
    ASSERT_EQ(distribution->getRank(3), 0);

}