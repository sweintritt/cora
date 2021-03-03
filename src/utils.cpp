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

std::vector<std::string> split(const std::string& value, const char delimiter) {
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream stream(value);

   while (std::getline(stream, token, delimiter)) {
      tokens.push_back(token);
   }

   return tokens;
}

std::string toString(const std::vector<std::string>& values) {
  std::ostringstream stream;

  if (!values.empty()) {
    std::copy(values.begin(), values.end() - 1,
        std::ostream_iterator<std::string>(stream, ","));

    stream << values.back();
  }

  return stream.str();
}