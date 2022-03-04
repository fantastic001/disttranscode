
#include <net/NodeComm.hpp>
#include <mpi.h>

#include <iostream>

using namespace dtcode::net;
using namespace std;

NodeComm::NodeComm(int from_rank, int to_rank, int size) : from_rank(from_rank), to_rank(to_rank), size(size) {

}

void NodeComm::send(string msg) {
   cout << "From: " << from_rank << " To: " << to_rank << " Message: " << msg << endl;
    if (to_rank < size) MPI_Send(msg.c_str(), msg.size(), MPI_BYTE, to_rank, 0, MPI_COMM_WORLD);
    else {
        for (int i = 0; i<size; i++) {
            if (i != from_rank) MPI_Send(msg.c_str(), msg.size(), MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }
    }
}


string NodeComm::receive() {
    int to = to_rank;
    if (to_rank == size) to = MPI_ANY_SOURCE;
    MPI_Status status;
    int temp;
    // Probe for matching message without receiving it
    MPI_Probe(to, 0, MPI_COMM_WORLD, &status);
    // Get the number of data elements in the message
    MPI_Get_count(&status, MPI_BYTE, &temp);
    char *var = new char[temp];
    MPI_Recv(var, temp, MPI_BYTE, to, 0, MPI_COMM_WORLD, &status);
    cout << "Received from=" << to_rank << " to="  << from_rank << " msg=" << string(var) << endl;
    return string(var);
}

NodeComm NodeComm::operator>>(vector<uint8_t>& value) {
    int to = to_rank;
    if (to_rank == size) to = MPI_ANY_SOURCE;
    MPI_Status status;
    int temp;
    // Probe for matching message without receiving it
    MPI_Probe(to, 0, MPI_COMM_WORLD, &status);
    // Get the number of data elements in the message
    MPI_Get_count(&status, MPI_BYTE, &temp);
    uint8_t *var = new uint8_t[temp];
    MPI_Recv(var, temp, MPI_BYTE, to, 0, MPI_COMM_WORLD, &status);
    for (int i = 0; i<temp; i++) {
        value.push_back(var[i]);
    }
    cout << "MSG,R," << to_rank << ","  << from_rank << ",";
    for (auto v : value) {
        cout << hex << (int) v << " ";
    }
    cout << endl;
    return NodeComm(from_rank, to_rank, size);
}

NodeComm NodeComm::operator<<(vector<uint8_t> value) {
    cout << "MSG,S," << from_rank << ","  << to_rank << ",";
    for (auto v : value) {
        cout << hex << (int) v << " ";
    }
    cout << endl;
    if (to_rank == size) {
        for (int i = 0; i<size; i++) {
            if (i != from_rank) {
                NodeComm(from_rank, i, size) << value;
            }
        }
        return *this;
    }
    uint8_t *data = new uint8_t[value.size()];
    copy(value.begin(), value.end(), data);
    MPI_Request request;
    MPI_Status status;
    MPI_Isend(data, value.size(), MPI_BYTE, to_rank, 0, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);
    return NodeComm(from_rank, to_rank, size);
}