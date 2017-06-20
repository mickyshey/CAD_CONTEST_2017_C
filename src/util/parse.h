/****************************************************************************
  FileName     [ parse.h ]
  PackageName  [ util ]
  Synopsis     [ parsing functions ]
  Author       [ music960633 ]
****************************************************************************/

#ifndef UTIL_PARSE_H_
#define UTIL_PARSE_H_

#include <string>
#include <vector>
#include <fstream>

std::string readUntil(std::ifstream& ifs, char end);

std::vector<std::string> split(const std::string& str, const std::string& sep);

std::string myToString(unsigned n);

bool str2Int(const std::string& str, int& num);

#endif  // UTIL_PARSE_H_
