#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
#include <vector>
#include <unordered_map>

namespace
{
typedef std::vector<std::vector<int>> Grid;

  void process_file(std::ifstream& input, Grid& grid)
  {
    do {
      char buf[5000];
      input.getline(buf, 5000);
      std::vector<int> row;
      std::stringstream row_str(buf);
      while (!row_str.eof()) {
        char c;
        row_str >> c;
        if (!row_str.eof()) {
          row.push_back(c - '0');
        }
      }
      grid.push_back(row);
    } while (!input.eof());
  }

  long long get_minimum_path_dp(const Grid& grid)
  {
    std::vector<long long> totalGridRow(grid[0].size(), 0);
    std::vector<std::vector<long long>> totalGrid(grid.size(), totalGridRow);
    for (int x = 1; x < grid.size(); ++x) {
      totalGrid[x][0] = grid[x][0] + totalGrid[x-1][0];
    }
    for (int y = 1; y < grid[0].size(); ++y) {
      totalGrid[0][y] = grid[0][y] + totalGrid[0][y-1];
    }
    for (int x = 1; x < grid.size(); ++x) {
      for (int y = 1; y < grid[0].size(); ++y) {
        totalGrid[x][y] = grid[x][y] + std::min(totalGrid[x-1][y], totalGrid[x][y-1]);
      }
    }
    
    return totalGrid[grid.size()-1][grid[0].size()-1];
  }

#if 0
  long long get_minimum_path_walk(const Grid& grid)
  {
    for (int x = 1; x < grid.size(); ++x) {
      totalGrid[x][0] = grid[x][0] + totalGrid[x-1][0];
    }
    for (int y = 1; y < grid[0].size(); ++y) {
      totalGrid[0][y] = grid[0][y] + totalGrid[0][y-1];
    }
    for (int x = 1; x < grid.size(); ++x) {
      for (int y = 1; y < grid[0].size(); ++y) {
        totalGrid[x][y] = grid[x][y] + std::min(totalGrid[x-1][y], totalGrid[x][y-1]);
      }
    }
    
    return totalGrid[grid.size()-1][grid[0].size()-1];
  }
#endif
}

int main()
{
  std::ifstream input("AoC_15_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  Grid grid;
  process_file(input, grid);
  
  std::cout << "Part One: " << get_minimum_path_dp(grid) << "\n";
  
  std::vector<int> finalGridRow(grid[0].size() * 5, 0);
  std::vector<std::vector<int>> finalGrid(grid.size() * 5, finalGridRow);
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      for (int x = 0; x < grid.size(); ++x) {
        for (int y = 0; y < grid[0].size(); ++y) {
          size_t target_x = i * grid.size() + x;
          size_t target_y = j * grid.size() + y;
          int risk;
          if (i > 0) {
            risk = finalGrid[target_x - grid.size()][target_y] % 9 + 1;
          }
          else if (j > 0) {
            risk = finalGrid[target_x][target_y - grid.size()] % 9 + 1;
          }
          else {
            risk = grid[x][y];
          }
          finalGrid[target_x][target_y] = risk;
        }
      }
    }
  }
  
  std::cout << "Part Two: " << get_minimum_path_dp(finalGrid) << std::endl;
  
  return 0;
}
