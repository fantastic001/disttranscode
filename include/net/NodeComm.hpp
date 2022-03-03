
#pragma once 

#include <string>
#include <sstream>
#include <vector>

namespace dtcode::net {
    class NodeComm {
            int from_rank, to_rank, size;
        public:
            NodeComm(int from_rank, int to_rank, int size);

            void send(std::string msg);
            std::string receive();

            template <typename T> 
            NodeComm operator<<(T value) {
                std::stringstream ss; 
                ss << value;
                send(ss.str());
                return NodeComm(from_rank, to_rank, size);
            }

            NodeComm operator<<(std::vector<uint8_t> value);

            template<class T>
            NodeComm operator>>(T& value) {
                std::stringstream ss(receive()); 
                ss >> value;
                return NodeComm(from_rank, to_rank, size);
            }

            NodeComm operator>>(std::vector<uint8_t>& value);

    };
}