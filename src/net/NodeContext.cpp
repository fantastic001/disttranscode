
#include <net/NodeContext.hpp>

#include <chrono>
#include <unistd.h>

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

void NodeContext::debug() 
{
    volatile int i = 0;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("PID %d on %s ready for attach\n", getpid(), hostname);
    fflush(stdout);
    while (0 == i)
        sleep(5);
}

void NodeContext::share(int source, int& variable) {
    MPI_Bcast(&variable, 1, MPI_INT, source, MPI_COMM_WORLD);
}