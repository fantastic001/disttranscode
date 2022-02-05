
#include <data/Frame.hpp>

using namespace std; 
using namespace dtcode::data;

std::vector<int> dtcode::data::pos2d(int row, int column) {
    return std::vector {row, column};
}
std::vector<int> dtcode::data::pos1d(int pos) {
    return std::vector {pos};
}