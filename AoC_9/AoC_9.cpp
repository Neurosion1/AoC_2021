#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <numeric>
#include <vector>

namespace
{
  typedef std::vector<std::vector<int>> Grid;
  typedef std::vector<std::vector<char>> CharGrid;

  void process_file(std::ifstream& input, Grid& grid)
  {
    char buf[5000];
    for (int x = 0; x < 100; ++x) {
      input.getline(buf, 5000);
      std::stringstream string_buffer(buf);
      std::vector<int> row;
      for (int y = 0; y < 100; ++y) {
        char c;
        string_buffer >> c;
        row.emplace_back(c - '0');
      };
      grid.emplace_back(row);
    };
  }

  bool is_local_minimum(const Grid& grid, int x, int y)
  {
    int test = grid[x][y];
    
    if (x > 0 && test >= grid[x-1][y]) {
      return false;
    }
    
    if (x < 99 && test >= grid[x+1][y]) {
      return false;
    }
    
    if (y > 0 && test >= grid[x][y-1]) {
      return false;
    }
    
    if (y < 99 && test >= grid[x][y+1]) {
      return false;
    }
    
    return true;
  }

  int fill(CharGrid& grid, int x, int y)
  {
    int retval = 0;
    
    if (grid[x][y] == '?') {
      retval = 1;
      grid[x][y] = 'O';
      if (x > 0 && grid[x-1][y] == '?') {
        retval += fill(grid, x - 1, y);
      }
      if (x < 99 && grid[x+1][y] == '?') {
        retval += fill(grid, x + 1, y);
      }
      if (y > 0 && grid[x][y-1] == '?') {
        retval += fill(grid, x, y - 1);
      }
      if (y < 99 && grid[x][y+1] == '?') {
        retval += fill(grid, x, y + 1);
      }
    }
    
    return retval;
  }
}

int main()
{
  std::ifstream input("AoC_9_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }

  Grid grid;
  process_file(input, grid);
  
  int part_one = 0;
  for (int x = 0; x < grid.size(); ++x) {
    for (int y = 0; y < grid.size(); ++y) {
      if (is_local_minimum(grid, x, y)) {
        part_one += (1 + grid[x][y]);
      }
    }
  }
  
  std::vector<char> row(100, '?');
  CharGrid chargrid(100, row);
  
  for (int x = 0; x < grid.size(); ++x) {
    for (int y = 0; y < grid.size(); ++y) {
      if (grid[x][y] == 9) chargrid[x][y] = 'X';
    }
  }
  
  std::vector<int> areas;
  for (int x = 0; x < chargrid.size(); ++x) {
    for (int y = 0; y < chargrid.size(); ++y) {
      if (chargrid[x][y] == '?') {
        int fillsize = fill(chargrid, x, y);
        if (fillsize > 0) {
          areas.push_back(fillsize);
        }
      }
    }
  }
  
  std::sort(areas.begin(), areas.end(), [](int x, int y) { return x > y; });
  
  for (int x = 0; x < grid.size(); ++x) {
    for (int y = 0; y < grid.size(); ++y) {
      if (is_local_minimum(grid, x, y)) {
        part_one += (1 + grid[x][y]);
      }
    }
  }
  
  std::cout << "Part One: " << part_one << std::endl;
  std::cout << "Part Two: " << areas[0] * areas[1] * areas[2] << std::endl;
  
  return 0;
}
