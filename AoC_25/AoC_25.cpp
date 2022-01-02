#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <queue>

namespace
{
  void process_file(std::ifstream& input, std::vector<std::string>& herds)
  {
    do {
      char buf[5000];
      input.getline(buf, 5000);
      herds.push_back(buf);
    } while (!input.eof());
  }

  bool move(std::vector<std::string>& start_herds)
  {
    bool something_moved = false;
    size_t length = start_herds.size(), width = start_herds[0].size();
    std::string row(start_herds[0].size(), '.');
    std::vector<std::string> east_herds(start_herds.size(), row);
    
    for (size_t i = 0; i < length; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (start_herds[i][j] == '>' && start_herds[i][(j + 1) % width] == '.') {
          east_herds[i][(j + 1) % width] = '>';
          east_herds[i][j] = '.';
          start_herds[i][(j + 1) % width] = 'X';
          something_moved = true;
        }
        else if (start_herds[i][j] != 'X'){
          east_herds[i][j] = start_herds[i][j];
        }
      }
    }
    
    for (size_t i = 0; i < length; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (east_herds[i][j] == 'v' && east_herds[(i + 1) % length][j] == '.') {
          start_herds[(i + 1) % length][j] = 'v';
          start_herds[i][j] = '.';
          east_herds[(i + 1) % length][j] = 'X';
          something_moved = true;
        }
        else if (east_herds[i][j] != 'X') {
          start_herds[i][j] = east_herds[i][j];
        }
      }
    }
    
    return something_moved;
  }
}

int main()
{
  std::ifstream input("AoC_25_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  std::vector<std::string> herds;
  process_file(input, herds);
  
  int count = 0;
  do {
    ++count;
  } while (move(herds));
  
  std::cout << "Part One: " << count << "\n";
  
  return 0;
}
