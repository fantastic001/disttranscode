#pragma once 


#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <utility>
#include "gmock/gmock.h"  // Brings in Google Mock.

using namespace dtcode::net;
using namespace dtcode::data;
using namespace std; 
using namespace testing;

static vector<vector<uint8_t>> messages;
static vector<int> senders;
static vector<int> receivers; 

static vector<string> string_messages;
static vector<int> string_senders;
static vector<int> string_receivers;

struct Network {
    int size; 
    int rank;

};

struct NetworkCommFake {
    shared_ptr<Network> net;
    int from, to;
    NetworkCommFake(shared_ptr<Network> net, int from, int to)  : net(net), from(from), to(to) {

    }

    NetworkCommFake& operator<<(vector<uint8_t> data) {
        if (to == net->size) {
            for (int i = 0; i<net->size; i++) {
                if (i != net->rank) {
                    NetworkCommFake(net, net->rank, i) << data;
                }
            }
            return *this;
        }
        ::messages.push_back(data);
        ::senders.push_back(from);
        ::receivers.push_back(to);
        return *this;
    }

    NetworkCommFake& operator>>(vector<uint8_t>& data) {
        for (int i = 0; i<::messages.size(); i++) {
            if (::senders[i] == to && (::receivers[i] == from || ::receivers[i] == net->size)) {
                data = ::messages[i];
                ::messages.erase(::messages.begin() + i);
                ::receivers.erase(::receivers.begin() + i);
                ::senders.erase(::senders.begin()+i);
                return *this;
            }
        }
        return *this;
    }

    template<class T> 
    NetworkCommFake& operator<<(const T& data) {
        if (to == net->size) {
            for (int i = 0; i<net->size; i++) {
                if (i != net->rank) {
                    NetworkCommFake(net, net->rank, i) << data;
                }
            }
            return *this;
        }
        stringstream ss;
        ss << data;
        ::string_messages.push_back(ss.str());
        ::string_senders.push_back(from);
        ::string_receivers.push_back(to);
        return *this;
    }

    template<class T> 
    NetworkCommFake& operator>>(T& data) {
        for (int i = 0; i<::string_messages.size(); i++) {
            if (::string_senders[i] == to && (::string_receivers[i] == from)) {
                stringstream ss(::string_messages[i]);
                ss >> data;
                std::cout << from << " Received " << string_messages[i] << endl;
                ::string_messages.erase(::string_messages.begin() + i);
                ::string_receivers.erase(::string_receivers.begin() + i);
                ::string_senders.erase(::string_senders.begin() + i);
                return *this;
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


    NetworkCommFake operator[] (int to) {
        return NetworkCommFake(net, net->rank, to);
    }

    NetworkCommFake operator() () {
        return NetworkCommFake(net, net->rank, net->size);
    }

    int size() {
        return net->size;
    }
    int rank() {
        return net->rank;
    }
};