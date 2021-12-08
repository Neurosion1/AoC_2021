#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace
{
  typedef std::string digit_desc;
  typedef std::vector<std::pair<std::array<digit_desc, 10>, std::array<digit_desc, 4>>> Digitlist;

  void process_file(std::ifstream& input, Digitlist& digitlist)
  {
    std::string delimeter;
    while (!input.eof()) {
      char buf[5000];
      input.getline(buf, 5000);
      std::stringstream string_buffer(buf);
      std::array<digit_desc, 10> signal;
      std::array<digit_desc, 4> output;
      for (int i = 0; i < 10; ++i) {
        digit_desc digits;
        string_buffer >> digits;
        signal[i] = digits;
      }
      string_buffer >> delimeter;
      for (int i = 0; i < 4; ++i) {
        digit_desc digits;
        string_buffer >> digits;
        output[i] = digits;
      }
      digitlist.emplace_back(std::make_pair(signal, output));
    };
  }

  std::array<std::string, 10> decode(std::array<std::string, 10> input)
  {
    std::vector<std::pair<std::string,int>> retval;
    std::array<std::string, 10> results;
    std::unordered_map<std::string, int> leftovers;
    for (int i = 0; i < 10; ++i) {
      std::sort(input[i].begin(), input[i].end());
      leftovers.insert(std::make_pair(input[i], i));
    }
    for (int i = 0; i < 10; ++i) {
      switch(input[i].size()) {
        case 2:
          results[1] = input[i];
          leftovers.erase(input[i]);
          break;
        case 3:
          results[7] = input[i];
          leftovers.erase(input[i]);
          break;
        case 4:
          results[4] = input[i];
          leftovers.erase(input[i]);
          break;
        case 7:
          results[8] = input[i];
          leftovers.erase(input[i]);
          break;
      }
    }
    
    // Find 3
    for (auto entry : leftovers) {
      if (entry.first.size() == 5 && entry.first.find(results[1]) != std::string::npos) {
        results[3] = entry.first;
        leftovers.erase(entry.first);
        break;
      }
    }
    
    // Find 9
    for (auto entry : leftovers) {
      if (entry.first.size() == 6 && entry.first.find(results[4]) != std::string::npos) {
        results[9] = entry.first;
        leftovers.erase(entry.first);
        break;
      }
    }
    
    // Find 0
    for (auto entry : leftovers) {
      if (entry.first.size() == 6 && entry.first.find(results[4]) == std::string::npos) {
        results[0] = entry.first;
        leftovers.erase(entry.first);
        break;
      }
    }
    
    // Find 6
    for (auto entry : leftovers) {
      if (entry.first.size() == 6) {
        results[6] = entry.first;
        leftovers.erase(entry.first);
        break;
      }
    }
    
    for (auto entry : leftovers) {
      std::vector<char> remaining_letter;
      std::set_difference(results[6].begin(), results[6].end(), entry.first.begin(), entry.first.end(), remaining_letter);
      if (results[1].find(remaining_letter[0]) != std::string::npos) {
        results[5] = entry.first;
        leftovers.erase(entry.first);
      }
    }
    
    results[2] = leftovers.begin()->first;
    
    return results;
  }
}

int main()
{
  std::ifstream input("AoC_8_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  Digitlist digitlist;
  process_file(input, digitlist);
  
  int part_one_count = 0;
  for (std::pair<std::array<digit_desc, 10>, std::array<digit_desc, 4>> entry : digitlist) {
    for (int i = 0; i < 4; ++i) {
      size_t entry_size = entry.second[i].size();
      if (entry_size == 2 ||
          entry_size == 3 ||
          entry_size == 4 ||
          entry_size == 7) {
        ++part_one_count;
      }
    }
  }
  
  std::cout << "Part One: " << part_one_count << std::endl;
  
  return 0;
}
