#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

typedef std::pair<std::pair<int, int>, std::pair<int, int>> Coordinates;

namespace
{
  void process_file(std::ifstream& input, std::vector<Coordinates>& coordinates)
  {
    char c;
    char arrow[5];
    int x1, x2, y1, y2;
    while (!input.eof()) {
      input >> x1 >> c >> y1 >> arrow >> x2 >> c >> y2;
      coordinates.emplace_back (std::make_pair(std::make_pair(x1, y1), std::make_pair(x2, y2)));
    };
  }

  int countIntersections(const std::vector<Coordinates> coordinates, bool count_diagonals)
  {
    std::array<int, 1000> oneLine;
    oneLine.fill(0);
    std::array<std::array<int, 1000>, 1000> grid;
    grid.fill(oneLine);
    
    int intersectionCount = 0;
    for (auto coord : coordinates) {
      int first_x = coord.first.first, first_y = coord.first.second;
      int last_x = coord.second.first, last_y = coord.second.second;
      if (coord.first.first == coord.second.first) {
        for (int y = first_y;
             first_y < last_y ? y <= last_y : y >= last_y;
             first_y < last_y ? ++y : --y) {
          if (grid[coord.first.first][y]++ == 1) {
            ++intersectionCount;
          }
        }
      }
      else if (coord.first.second == coord.second.second) {
        for (int x = first_x;
             first_x < last_x ? x <= last_x : x >= last_x;
             first_x < last_x ? ++x : --x) {
          if (grid[x][coord.first.second]++ == 1) {
            ++intersectionCount;
          }
        }
      }
      else if (count_diagonals) {
        for (int x = first_x, y = first_y;
             first_x < last_x ? x <= last_x : x >= last_x;
             first_x < last_x ? ++x : --x, first_y < last_y ? ++y : --y) {
          if (grid[x][y]++ == 1) {
            ++intersectionCount;
          }
        }
      }
    }
    
    return intersectionCount;
  }
}
int main()
{
  std::ifstream input("AoC_5_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  std::vector<Coordinates> coordinates;
  process_file(input, coordinates);
  
  std::cout << "Part One:" << countIntersections(coordinates, false) << std::endl;
  std::cout << "Part Two:" << countIntersections(coordinates,  true) << std::endl;
  
  return 0;
}
