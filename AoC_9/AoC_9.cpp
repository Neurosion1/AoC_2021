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

namespace
{
  typedef std::vector<std::vector<int>> Grid;
  typedef std::vector<std::vector<char>> CharGrid;

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

  bool is_local_minimum(const Grid& grid, int x, int y)
  {
    int test = grid[x][y];
    
    if (x > 0 && test >= grid[x-1][y]) {
      return false;
    }
    if (x < grid.size() - 1 && test >= grid[x+1][y]) {
      return false;
    }
    if (y > 0 && test >= grid[x][y-1]) {
      return false;
    }
    if (y < grid[0].size() - 1 && test >= grid[x][y+1]) {
      return false;
    }
    
    return true;
  }

  int fill(CharGrid& grid, int startx, int starty, char unfilled_char, char fill_char)
  {
    int retval = 0;
    
    std::queue<std::pair<int, int>> pixels;
    pixels.push({startx, starty});
    while (!pixels.empty()) {
      int x = pixels.front().first;
      int y = pixels.front().second;
      pixels.pop();
      if (grid[x][y] == unfilled_char) {
        ++retval;
        grid[x][y] = fill_char;
        if (x > 0) {
          pixels.push( { x - 1, y } );
        }
        if (x < grid.size() - 1) {
          pixels.push( { x + 1, y });
        }
        if (y > 0) {
          pixels.push( { x, y - 1 });
        }
        if (y < grid[0].size() - 1) {
          pixels.push( { x, y + 1});
        }
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
  
  std::vector<std::pair<int, int>> lowPoints;
  int part_one = 0;
  for (int x = 0; x < grid.size(); ++x) {
    for (int y = 0; y < grid.size(); ++y) {
      if (is_local_minimum(grid, x, y)) {
        part_one += (1 + grid[x][y]);
        lowPoints.push_back( { x, y } );
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
  
  // Interestingly it looks like the input is designed so that every
  // basin has exactly one low point; every basin continuously
  // descends from '9' to '0'. There are no intemediate bowls, like
  // in the following:
  //
  //    9999999
  //    9444449
  //    9414149 <-- Two low points within this single basin,
  //    9444449     which doesn't seem to happen.
  //    9999999
  //
  // This doesn't really affect the solution, but I imagine it might
  // become important in a future puzzle.
  //
  std::multimap<int, std::pair<int,int>, std::greater<int>> areas;
  for (auto point : lowPoints) {
    int fillsize = fill(chargrid, point.first, point.second, '?', 'O');
    if (fillsize > 0) {
      areas.insert({ fillsize, point });
    }
  }
  
  auto iter = areas.begin();
  auto first = *iter, second = *++iter, third = *++iter;
  
  std::cout << "Part One: " << part_one << std::endl;
  std::cout << "Part Two: " << first.first * second.first * third.first << std::endl;
  std::cout << std::endl;
  
  // Let's visualize it for giggles.
  fill(chargrid, first.second.first,  first.second.second,  'O', ' ');
  fill(chargrid, second.second.first, second.second.second, 'O', ' ');
  fill(chargrid, third.second.first,  third.second.second,  'O', ' ');
  for (const auto & row : chargrid) {
    for (char c : row) {
      std::cout << c;
    }
    std::cout << std::endl;
  }
  
  return 0;
}
