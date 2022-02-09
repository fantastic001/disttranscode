
#pragma once 

#include <mpi.h>

#include <net/NodeComm.hpp>


namespace dtcode::net {

    class NodeContext {
        public:
            NodeContext(int* argc, char*** argv);
            virtual ~NodeContext();

            int size();
            int rank();

            template<class T>
            void root(T callable) {
                if (rank() == 0) callable();
            }

            NodeComm operator[] (int _rank) {
                return NodeComm(rank(), _rank);
            }
    };
}