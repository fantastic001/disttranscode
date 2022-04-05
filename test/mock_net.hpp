#pragma once 


#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <utility>
#include "gmock/gmock.h"  // Brings in Google Mock.

using namespace std; 
using namespace testing;

vector<vector<uint8_t>>& get_messages();
vector<int>& get_senders();
vector<int>& get_receivers(); 

vector<string>& get_string_messages();
vector<int>& get_string_senders();
vector<int>& get_string_receivers();

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
        ::get_messages().push_back(data);
        ::get_senders().push_back(from);
        ::get_receivers().push_back(to);
        return *this;
    }

    NetworkCommFake& operator>>(vector<uint8_t>& data) {
        for (int i = 0; i<::get_messages().size(); i++) {
            if (::get_senders()[i] == to && (::get_receivers()[i] == from || ::get_receivers()[i] == net->size)) {
                data = ::get_messages()[i];
                ::get_messages().erase(::get_messages().begin() + i);
                ::get_receivers().erase(::get_receivers().begin() + i);
                ::get_senders().erase(::get_senders().begin()+i);
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
        ::get_string_messages().push_back(ss.str());
        ::get_string_senders().push_back(from);
        ::get_string_receivers().push_back(to);
        std::cout << from << " sent message to " << to << ": " << data << "\n";
        cout << "Buffer size=" << get_string_messages().size() << endl;
        return *this;
    }

    template<class T> 
    NetworkCommFake& operator>>(T& data) {
        std::cout << "Buffer size " << get_string_messages().size() << endl;
        for (int i = 0; i<::get_string_messages().size(); i++) {
            if (::get_string_senders()[i] == to && (::get_string_receivers()[i] == from)) {
                stringstream ss(::get_string_messages()[i]);
                ss >> data;
                std::cout << from << " Received " << get_string_messages()[i] << endl;
                ::get_string_messages().erase(::get_string_messages().begin() + i);
                ::get_string_receivers().erase(::get_string_receivers().begin() + i);
                ::get_string_senders().erase(::get_string_senders().begin() + i);
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

    void share(int sender, int& variable) {

    }
};