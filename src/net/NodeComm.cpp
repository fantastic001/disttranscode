
#include <net/NodeComm.hpp>
#include <mpi.h>
using namespace dtcode::net;
using namespace std;

NodeComm::NodeComm(int from_rank, int to_rank) : from_rank(from_rank), to_rank(to_rank) {

}

void NodeComm::send(string msg) {
    MPI_Send(msg.c_str(), msg.size(), MPI_BYTE, to_rank, 0, MPI_COMM_WORLD);
}


string NodeComm::receive() {
    MPI_Status status;
    int temp;
    // Probe for matching message without receiving it
    MPI_Probe(to_rank, 0, MPI_COMM_WORLD, &status);
    // Get the number of data elements in the message
    MPI_Get_count(&status, MPI_BYTE, &temp);
    char *var = new char[temp];
    MPI_Recv(var, temp, MPI_BYTE, to_rank, 0, MPI_COMM_WORLD, &status);
    return string(var);
}