#include "utils.hpp"

#include <sstream>
#include <algorithm>
#include <iterator>

std::vector<std::string> split(const std::string& value) {
    std::vector<std::string> tokens;
    std::istringstream sstream(value);
    std::copy(std::istream_iterator<std::string>(sstream),
         std::istream_iterator<std::string>(),
         std::back_inserter(tokens));
    return tokens;
}