#include "utils.hpp"

#include <plog/Log.h>

#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <string>
#include <cstdio>
#include <ctime>

const std::string DATE_TIME_FORMAT = "%Y-%m-%d %H-%M-%S";

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

void ltrim(std::string& value) {
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

void rtrim(std::string& value) {
    value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), value.end());
}

void trim(std::string& value) {
    ltrim(value);
    rtrim(value);
}

std::vector<std::string> toStringVector(int length, char* values[]) {
    std::vector<std::string> result;

    for (int i = 0; i < length; ++i) {
        result.emplace_back(values[i]);
    }

    return result;
}
