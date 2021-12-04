#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>

namespace
{
  void process_file(std::ifstream& input, std::vector<std::string>& report, std::vector<int>& zeroes, std::vector<int>& ones)
  {
    size_t length = 0;
    while (!input.eof()) {
      std::string line;
      input >> line;
      report.emplace_back(line);
      if (length == 0) {
        length = line.size();
        zeroes.resize(length, 0);
        ones.resize(length, 0);
      }
      for (size_t i = 0; i < line.size(); ++i) {
        ++(line[i] == '0' ? zeroes : ones)[i];
      }
    };
  }

  int AoC_3_1(std::vector<int> zeroes, std::vector<int> ones)
  {
    int gamma = 0, epsilon = 0;
    for (size_t i = 0; i < zeroes.size(); ++i) {
      if (ones[zeroes.size() - (i + 1)] > zeroes[zeroes.size() - (i + 1)]) {
        gamma += (1 << i);
      }
      else {
        epsilon += (1 << i);
      }
    }

    return gamma * epsilon;
  }

  int process_bit_criteria(std::vector<std::string> report, std::vector<int> zeroes, std::vector<int> ones, bool most_common)
  {
    size_t line_size = report[0].size();
    int current_bit = 0;
    while (report.size() > 1) {
      char which_to_keep = (most_common == (ones[current_bit] >= zeroes[current_bit])) ? '1' : '0';
      report.erase(std::remove_if(report.begin(), report.end(),
        [current_bit, which_to_keep, line_size, & zeroes, & ones](const std::string& line) {
          if (line[current_bit] != which_to_keep) {
            for (size_t i = current_bit; i < line_size; ++i) {
              --(line[i] == '0' ? zeroes : ones)[i];
            }
            return true;
          }
          return false;
        }), report.end());
      ++current_bit;
    }

    int result = 0;
    for (size_t i = 0; i < line_size; ++i) {
      if (report[0][line_size - (i + 1)] == '1') {
        result += (1 << i);
      }
    }

    return result;
  }

  int AoC_3_2(std::vector<std::string> report, std::vector<int> zeroes, std::vector<int> ones)
  {
    int oxygen_rating = process_bit_criteria(report, zeroes, ones, true);
    int scrubber_rating = process_bit_criteria(report, zeroes, ones, false);

    return oxygen_rating * scrubber_rating;
  }
}

int main()
{
  std::ifstream input("AoC_3_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }

  std::vector<std::string> report;
  std::vector<int> zeroes, ones;
  process_file(input, report, zeroes, ones);

  std::cout << "Part 1: " << AoC_3_1(zeroes, ones) << std::endl;
  std::cout << "Part 2: " << AoC_3_2(report, zeroes, ones) << std::endl;

  return 0;
}
