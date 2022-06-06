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

/**
 * Convert the given arguments array provided by the main method into a
 * more easy to use string vector.
 * @param[in] length length of the array.
 * @param[in] values array with values.
 */
std::vector<std::string> toStringVector(int length, char* values[]);