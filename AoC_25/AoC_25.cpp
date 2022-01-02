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

  bool move(std::vector<std::string>& herds)
  {
    bool something_moved = false;
    size_t length = herds.size(), width = herds[0].size();
    std::string row(herds[0].size(), '.');
    std::vector<std::string> work_herds(herds.size(), row);
    
    for (size_t i = 0; i < length; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (herds[i][j] == '>' && herds[i][(j + 1) % width] == '.') {
          work_herds[i][(j + 1) % width] = '>';
          work_herds[i][j] = '.';
          herds[i][(j + 1) % width] = 'X';
          something_moved = true;
        }
        else if (herds[i][j] != 'X'){
          work_herds[i][j] = herds[i][j];
        }
      }
    }
    
    for (size_t i = 0; i < length; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (work_herds[i][j] == 'v' && work_herds[(i + 1) % length][j] == '.') {
          herds[(i + 1) % length][j] = 'v';
          herds[i][j] = '.';
          work_herds[(i + 1) % length][j] = 'X';
          something_moved = true;
        }
        else if (work_herds[i][j] != 'X') {
          herds[i][j] = work_herds[i][j];
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
