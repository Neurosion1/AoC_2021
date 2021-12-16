#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
typedef std::vector<std::vector<char>> Grid;

void process_file(std::ifstream& input, Grid& grid, std::vector<std::pair<char, int>>& folds)
{
  int max_x = 0, max_y = 0;
  std::vector<std::pair<int, int>> points;
  do {
    char buffer[5000];
    input.getline(buffer, 5000);
    std::string thisLine = buffer;
    if (thisLine.empty()) {
      break;
    }
    else {
      std::stringstream str(thisLine);
      int x, y;
      char comma;
      str >> x >> comma >> y;
      points.push_back(std::make_pair(x, y));
      max_x = std::max(x, max_x);
      max_y = std::max(y, max_y);
    }
  } while (true);

  std::vector<char> row(max_x + 1, ' ');
  grid.resize(max_y + 1, row);
  for (auto point : points) {
    grid[point.second][point.first] = '#';
  }
  
  do {
    std::string fold, along;
    char axis, equals;
    int location;
    input >> fold >> along >> axis >> equals >> location;
    folds.push_back(std::make_pair(axis, location));
  } while (!input.eof());
}
}

void fold(Grid& grid, char axis, int fold)
{
  if (axis == 'x') {
    for (size_t x = fold + 1, step = 2; x < grid[0].size(); ++x, step += 2) {
      for (size_t y = 0; y < grid.size(); ++y) {
        if (grid[y][x] == '#' || grid[y][x - step] == '#') {
          grid [y][x - step] = '#';
        }
      }
    }
    for (size_t y = 0; y < grid.size(); ++y) {
      grid[y].resize(fold);
    }
  }
  else {
    for (size_t y = fold + 1, step = 2; y < grid.size(); ++y, step += 2) {
      for (size_t x = 0; x < grid[0].size(); ++x) {
        if (grid[y][x] == '#' || grid[y - step][x] == '#') {
          grid [y - step][x] = '#';
        }
      }
    }
    grid.resize(fold);
  }
}

int main()
{
  std::ifstream input("AoC_13_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  Grid grid;
  std::vector<std::pair<char, int>> folds;
  process_file(input, grid, folds);
  
  fold(grid, folds[0].first, folds[0].second);
  
  int dot_count = 0;
  for (size_t y = 0; y < grid.size(); ++y) {
    for (size_t x = 0; x < grid[0].size(); ++x) {
      if (grid[y][x] == '#') { ++dot_count; }
    }
  }
  
  std::cout << "Part One: " << dot_count << std::endl;
  
  for (size_t i = 1; i < folds.size(); ++i) {
    fold(grid, folds[i].first, folds[i].second);
  }
  
  std::cout << "Part Two: " << "\n\n";
  for (size_t y = 0; y < grid.size(); ++y) {
    for (size_t x = 0; x < grid[0].size(); ++x) {
      std::cout << grid[y][x];
    }
    std::cout << "\n";
  }
  std::cout << "\n";
  
  return 0;
}
