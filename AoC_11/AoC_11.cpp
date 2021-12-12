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
#include <map>
#include <queue>

namespace {
  typedef std::vector<std::vector<int>> Grid;

  void process_file(std::ifstream& input, Grid& grid)
  {
    while (!input.eof()) {
      char buf[5000];
      input.getline(buf, 5000);
      std::stringstream string_buffer(buf);
      std::vector<int> row;
      char c;
      string_buffer >> c;
      do {
        row.push_back(c - '0');
        string_buffer >> c;
      } while (!string_buffer.eof());
      grid.emplace_back(row);
    };
  }

  void increase(Grid& grid, int x, int y)
  {
    if (x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size() && grid[x][y] % 10 != 0) {
      ++grid[x][y];
    }
  }

  void flash(Grid& grid, int x, int y)
  {
    grid[x][y] = 0;
    increase(grid, x - 1, y - 1);
    increase(grid, x,     y - 1);
    increase(grid, x + 1, y - 1);
    increase(grid, x + 1, y);
    increase(grid, x + 1, y + 1);
    increase(grid, x,     y + 1);
    increase(grid, x - 1, y + 1);
    increase(grid, x - 1, y);
  }

  long process_step(Grid& grid)
  {
    bool flashed = false;
    long total_flashes = 0;
    do {
      flashed = false;
      for (int x = 0; x < grid.size(); ++x) {
        for (int y = 0; y < grid[0].size(); ++y) {
          if (grid[x][y] == 10) {
            flashed = true;
            flash(grid, x, y);
            ++total_flashes;
          }
        }
      }
    } while (flashed);
    return total_flashes;
  }
}

int main()
{
  std::ifstream input("AoC_11_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  Grid grid;
  process_file(input, grid);
  
  int i = 0;
  long part_one = 0;
  while(true)
  {
    for (int x = 0; x < grid.size(); ++x) {
      for (int y = 0; y < grid[0].size(); ++y) {
        ++grid[x][y];
      }
    }
    long these_flashes = process_step(grid);
    if (++i <= 100) {
      part_one += these_flashes;
    }
    if (these_flashes == 100) {
      break;
    }
  }
  
  std::cout << "Part One: " << part_one << std::endl;
  std::cout << "Part Two: " << i << std::endl;
  
  return 0;
}
