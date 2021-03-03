#pragma once

#include <vector>
#include <string>

std::vector<std::string> split(const std::string& value);

std::vector<std::string> split(const std::string& value, const char delimiter);

std::string toString(const std::vector<std::string>& values);