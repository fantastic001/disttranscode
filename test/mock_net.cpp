#include "mock_net.hpp"

static vector<vector<uint8_t>> messages;
static vector<int> senders;
static vector<int> receivers; 

static vector<string> string_messages;
static vector<int> string_senders;
static vector<int> string_receivers;


vector<vector<uint8_t>>& get_messages() {
    return ::messages;
}
vector<int>& get_senders() {
    return ::senders;
}
vector<int>& get_receivers() {
    return receivers;
}

vector<string>& get_string_messages() {
    return string_messages;
}
vector<int>& get_string_senders() {
    return string_senders;
}
vector<int>& get_string_receivers() {
    return string_receivers;
}