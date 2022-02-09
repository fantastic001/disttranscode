
#pragma once 

#include <string>
#include <sstream>

namespace dtcode::net {
    class NodeComm {
            int from_rank, to_rank;
        public:
            NodeComm(int from_rank, int to_rank);

            void send(std::string msg);
            std::string receive();

            template <typename T> 
            NodeComm operator<<(T value) {
                std::stringstream ss; 
                ss << value;
                send(ss.str());
                return NodeComm(from_rank, to_rank);
            }

            template<class T>
            NodeComm operator>>(T& value) {
                std::stringstream ss(receive()); 
                ss >> value;
                return NodeComm(from_rank, to_rank);
            }

    };
}