
#include <net/UniformFloodingConsensus.hpp>
#include "mocks.hpp"
#include "mock_net.hpp"

TEST(TestUniformFloodingConsensusBasic, test_creation) {
    auto net = make_shared<Network>();
    auto context = make_shared<NetworkFake>(net);
    ConsensusPtr consensus = make_shared<dtcode::net::UniformFloodingConsensus<NetworkFake>>(context);
}


class UniformFloodingConsensusTest : public ::testing::Test {
 protected:
    void SetUp() override {
            size = 3;
            ::string_messages.clear();
            ::string_senders.clear();
            ::string_receivers.clear();
            ::messages.clear();
            ::receivers.clear();
            ::senders.clear();
            for (int i = 0; i<3; i++) {
                net[i] = make_shared<Network>();
                net[i]->rank = i; 
                net[i]->size = size;
                context[i] = make_shared<NetworkFake>(net[i]);
                consensus[i] = make_shared<UniformFloodingConsensus<NetworkFake>>(context[i]);
            }
            
    }
    shared_ptr<Network> net[3];
    shared_ptr<NetworkFake> context[3];
    ConsensusPtr consensus[3];
    int size;
};

TEST_F(UniformFloodingConsensusTest, test_proposal_will_send_message_to_everyone) {
    consensus[0]->propose(0);
    ASSERT_EQ(::string_messages.size(), 4);
    ASSERT_EQ(::string_senders[0], 0);
    ASSERT_EQ(::string_senders[1], 0);
    ASSERT_EQ(::string_receivers[0], 1);
    ASSERT_EQ(::string_receivers[1], 2);
    ASSERT_EQ(::string_receivers[2], 1);
    ASSERT_EQ(::string_receivers[3], 2);
}


TEST_F(UniformFloodingConsensusTest, test_decision_is_minimal_number) {
    for (int i = 0; i<size; i++) {
        consensus[i]->propose(i+10);
    }
    ASSERT_EQ(::string_messages.size(), 12);
    ASSERT_EQ(::string_receivers[0], 1);
    ASSERT_EQ(::string_receivers[1], 2);
    ASSERT_EQ(::string_receivers[2], 1);
    ASSERT_EQ(::string_receivers[3], 2);
    ASSERT_EQ(::string_receivers[4], 0);
    ASSERT_EQ(::string_receivers[5], 2);
    ASSERT_EQ(::string_receivers[6], 0);
    ASSERT_EQ(::string_receivers[7], 2);
    ASSERT_EQ(::string_receivers[8], 0);
    ASSERT_EQ(::string_receivers[9], 1);
    ASSERT_EQ(::string_receivers[10], 0);
    ASSERT_EQ(::string_receivers[11], 1);
    ASSERT_EQ(::string_messages[0], "PROPOSAL ");
    ASSERT_EQ(::string_messages[2], "10");
    ASSERT_EQ(::string_messages[3], "10");
    ASSERT_EQ(::string_messages[6], "11");
    ASSERT_EQ(::string_messages[7], "11");
    ASSERT_EQ(::string_messages[10], "12");
    ASSERT_EQ(::string_messages[11], "12");


    ASSERT_EQ(consensus[0]->getDecision(), 10);
    ASSERT_EQ(consensus[1]->getDecision(), 10);
    ASSERT_EQ(consensus[2]->getDecision(), 10);
}