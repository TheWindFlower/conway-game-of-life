#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <tuple>
#include <string>

std::vector<std::tuple<int, int, bool>> next_generation(const std::vector<std::tuple<int, int, bool>> &current_status, int rows, int columns);
std::vector<std::tuple<int, int, bool>> start(std::string filename, int rows, int columns);
void grid_gen(int x, int y);
#endif
