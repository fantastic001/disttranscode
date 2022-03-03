
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

            void debug();

            template<class T>
            void root(T callable) {
                if (rank() == 0) callable();
            }

            NodeComm operator[] (int _rank) {
                return NodeComm(rank(), _rank, size());
            }

            NodeComm operator() () {
                return NodeComm(rank(), size(), size());
            }

            NodeContext& operator>> (std::vector<std::uint8_t>& arr) {
                int _size;
                if (rank() == 0) {
                    _size = arr.size();
                }
                MPI_Bcast(&_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
                uint8_t* my_arr = new uint8_t[_size];
                if (rank() == 0) {
                    std::copy(arr.begin(), arr.end(), my_arr);
                }               
                MPI_Bcast(my_arr, _size, MPI_BYTE, 0, MPI_COMM_WORLD);
                if (rank() != 0) {
                    arr.resize(_size);
                    std::copy(my_arr, my_arr+_size, arr.begin());
                }
                return *this;
            }
    };
}