
#include <data/Frame.hpp>

using namespace std; 

std::vector<int> pos2d(int row, int column) {
    return std::vector {row, column};
}
std::vector<int> pos1d(int pos) {
    return std::vector {pos};
}