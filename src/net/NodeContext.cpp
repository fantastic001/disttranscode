
#include <net/NodeContext.hpp>

using namespace dtcode::net;

NodeContext::NodeContext(int* argc, char*** argv) {
    MPI_Init(argc, argv);
}

NodeContext::~NodeContext() {
    MPI_Finalize();
}

int NodeContext::size() {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    return size;
}

int NodeContext::rank() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
}

