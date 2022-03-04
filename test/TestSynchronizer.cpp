
#include <net/ConsensusFactory.hpp>
#include <net/DistributionFactory.hpp>
#include <net/Synchronizer.hpp>
#include "mocks.hpp"
#include "mock_net.hpp"
#include <frame/FrameF.hpp>

using namespace dtcode::net;
using namespace dtcode::data;
using namespace dtcode::frame;

TEST(TestSynchronizerBasic, test_creation) {
    auto consensusFactory = make_shared<MockConsensusFactory>();
    auto distributionFactory = make_shared<MockDistributionFactory>();

    Synchronizer synchronizer(distributionFactory, consensusFactory);
}


class SynchronizerTest : public ::testing::Test {
 protected:
    void SetUp() override {
        distribution = make_shared<MockDistribution>();
        consensus = make_shared<MockConsensus>();
        consensusFactory = make_shared<MockConsensusFactory>();
        distributionFactory = make_shared<MockDistributionFactory>();
        EXPECT_CALL(*distributionFactory, create()).WillRepeatedly(Return(distribution));
        EXPECT_CALL(*consensusFactory, create()).WillRepeatedly(Return(consensus));

        // frame = make_shared<MockFrame>();
        // EXPECT_CALL(*frame, getDim()).WillRepeatedly(Return(vector<int> {10, 10} ));
        // EXPECT_CALL(*frame, getData(_,_,_)).WillRepeatedly(Return(0));
        // EXPECT_CALL(*frame, getChannelCount()).WillRepeatedly(Return(3));
        // EXPECT_CALL(*frame, isKeyFrame()).WillRepeatedly(Return(true));

        synchronizer = make_shared<Synchronizer>(distributionFactory, consensusFactory);


    }
    shared_ptr<MockConsensusFactory> consensusFactory;
    shared_ptr<MockDistributionFactory> distributionFactory;
    shared_ptr<MockDistribution> distribution;
    shared_ptr<MockConsensus> consensus;
    shared_ptr<Synchronizer> synchronizer;
    std::shared_ptr<MockFrame>  frame;
};


TEST_F(SynchronizerTest, test_will_call_distribute) {
    auto stream = make_shared<MockStream>();
    EXPECT_CALL(*distribution, distribute(_)).Times(1).WillOnce(Return(stream));
    EXPECT_CALL(*distribution, nextIndex()).Times(Exactly(1)).WillOnce(Return(-1));
    
    synchronizer->process(stream);
}



TEST_F(SynchronizerTest, test_will_decode_encode_filter) {
    auto stream = make_shared<MockStream>();
    auto segment = make_shared<MockSegment>();
    auto frame = dtcode::frame::F(10, 10, [] (int channel, int y, int x) {
        return 0;
    });
    EXPECT_CALL(*segment, nextFrame()).Times(Exactly(2))
        .WillOnce(Return(optional(frame)))
        .WillRepeatedly(Return(optional<FramePtr>()));
    EXPECT_CALL(*stream, parse()).WillOnce(Return(list<SegmentPtr>{segment}));
    EXPECT_CALL(*distribution, distribute(_)).WillOnce(Return(stream));
    EXPECT_CALL(*distribution, getSegment(0, _)).WillRepeatedly(Return(segment));
    EXPECT_CALL(*distribution, getSegment(1, _)).Times(Exactly(0));
    auto filter = make_shared<MockFilter>();
    synchronizer->addFilter(filter);
    EXPECT_CALL(*filter, filter(frame)).Times(Exactly(1)).WillOnce(Return(frame));
    EXPECT_CALL(*distribution, nextIndex()).Times(Exactly(2))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(-1));
    EXPECT_CALL(*consensus, propose(_)).Times(Exactly(2));
    EXPECT_CALL(*consensus, getDecision()).Times(Exactly(2))
        .WillOnce(Return(0))
        .WillOnce(Return(-1));
    synchronizer->process(stream);
}

TEST_F(SynchronizerTest, test_two_synchronizers) {
    auto stream = make_shared<MockStream>();
    auto segment = make_shared<MockSegment>();
    auto segment1 = make_shared<MockSegment>();
    auto frame = dtcode::frame::F(10, 10, [] (int channel, int y, int x) {
        return 0;
    });
    EXPECT_CALL(*segment, nextFrame()).Times(Exactly(2))
        .WillOnce(Return(frame))
        .WillRepeatedly(Return(optional<FramePtr>()));
    EXPECT_CALL(*segment1, nextFrame()).Times(Exactly(2))
        .WillOnce(Return(frame))
        .WillRepeatedly(Return(optional<FramePtr>()));

    auto distributionFactory1 = make_shared<MockDistributionFactory>();
    auto consensusFactory1 = make_shared<MockConsensusFactory>();
    auto consensus1 = make_shared<MockConsensus>();
    auto distribution1 = make_shared<MockDistribution>();
    
    EXPECT_CALL(*stream, parse()).WillOnce(Return(list<SegmentPtr> {
        segment,
        segment1
    }));

    EXPECT_CALL(*distributionFactory1, create()).WillOnce(Return(distribution1));
    EXPECT_CALL(*consensusFactory1, create()).WillOnce(Return(consensus1));
    EXPECT_CALL(*distribution1, distribute(_)).Times(Exactly(1)).WillOnce(Return(stream));
    EXPECT_CALL(*distribution1, getSegment(0, _)).WillRepeatedly(Return(segment));
    EXPECT_CALL(*distribution1, getSegment(1, _)).WillRepeatedly(Return(segment1));
    EXPECT_CALL(*distribution1, nextIndex()).Times(Exactly(2)).WillOnce(Return(1)).WillRepeatedly(Return(-1));

    EXPECT_CALL(*consensus1, propose(_)).Times(Exactly(3));
    EXPECT_CALL(*consensus1, getDecision()).Times(3)
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(-1));

    
    EXPECT_CALL(*distribution, getSegment(0, _)).WillRepeatedly(Return(segment));
    EXPECT_CALL(*distribution, getSegment(1, _)).WillRepeatedly(Return(segment1));
    EXPECT_CALL(*distribution, nextIndex()).Times(Exactly(2)).WillOnce(Return(0)).WillRepeatedly(Return(-1));
    EXPECT_CALL(*consensus, propose(_)).Times(Exactly(3));
    EXPECT_CALL(*consensus, getDecision()).Times(Exactly(3))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(-1));
    EXPECT_CALL(*distribution, distribute(_)).WillOnce(Return(stream));
    auto filter = make_shared<MockFilter>();

    auto synchronizer1 = make_shared<Synchronizer>(distributionFactory1, consensusFactory1, false);

    synchronizer1->addFilter(filter);
    synchronizer->addFilter(filter);
    EXPECT_CALL(*filter, filter(frame)).Times(Exactly(2)).WillRepeatedly(Return(frame));
    synchronizer->process(stream);
    synchronizer1->process(stream);
}