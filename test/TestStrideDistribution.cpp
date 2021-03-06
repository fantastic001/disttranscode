
#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <utility>
#include <net/StrideDistribution.hpp>
#include "gmock/gmock.h"  // Brings in Google Mock.

#include "mock_net.hpp"
#include "mocks.hpp"

using namespace dtcode::net;
using namespace dtcode::data;
using namespace std; 
using namespace testing;

class StrideDistributionTest : public ::testing::Test {
 protected:
    void SetUp() override {
            net = make_shared<Network>();
            net->size = 3;
            net->rank = 0;
            context = make_shared<NetworkFake>(net);
            distribution = make_shared<StrideDistribution<NetworkFake>>(context);
            ::get_messages().clear();
            ::get_senders().clear();
            ::get_receivers().clear();
            ::get_string_messages().clear();
            ::get_string_receivers().clear();
            ::get_string_senders().clear();
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
    ASSERT_EQ(::get_messages().size(), 0);
}


TEST_F(StrideDistributionTest, distribute_numbver_of_messages_equals_number_of_segments) {
    auto segment = make_shared<MockSegment>();
    EXPECT_CALL(*segment, serialize()).WillOnce(Return(vector<uint8_t> {1,2,3}));
    auto stream = make_shared<MockStream>();
    EXPECT_CALL(*stream, parse()).WillOnce(Return(list<SegmentPtr> {segment}));
    distribution->distribute(stream);
    ASSERT_EQ(::get_messages().size(), 0);
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
    ASSERT_EQ(::get_messages().size(), 4);
    EXPECT_EQ(::get_receivers()[0], 1);
    EXPECT_EQ(::get_receivers()[1], 1);
    EXPECT_EQ(::get_receivers()[2], 2);
    EXPECT_EQ(::get_receivers()[3], 2);
    EXPECT_EQ(::get_senders()[3], 0);
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


TEST_F(StrideDistributionTest, distribute_getsegment_local) {
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
    ASSERT_EQ(distribution->getSegment(0), segment0);
}


TEST_F(StrideDistributionTest, distribute_getsegment_remote) {
    auto net2 = make_shared<Network>();
    net2->rank = 1; 
    net2->size = 3;
    auto context2 = make_shared<NetworkFake>(net2);
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

    auto distribution2 = make_shared<StrideDistribution<NetworkFake>>(context2);
    distribution->distribute(stream);
    distribution2->distribute(nullptr);
    ASSERT_EQ(::get_string_messages().size(), 1);
    ASSERT_EQ(::get_messages().size(), 2);
    ASSERT_EQ(distribution->getSegment(1)->serialize()[0], 0);
}