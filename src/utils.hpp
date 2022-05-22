#pragma once

#include <vector>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>

std::vector<std::string> split(const std::string& value);

std::vector<std::string> split(const std::string& value, const char delimiter);

std::string toString(const std::vector<std::string>& values);

void ltrim(std::string& value);

void rtrim(std::string& value);

void trim(std::string& value);
