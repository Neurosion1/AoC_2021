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
#include <unordered_set>

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
        std::sort(digits.begin(), digits.end());
        signal[i] = digits;
      }
      string_buffer >> delimeter;
      for (int i = 0; i < 4; ++i) {
        digit_desc digits;
        string_buffer >> digits;
        std::sort(digits.begin(), digits.end());
        output[i] = digits;
      }
      digitlist.emplace_back(std::make_pair(signal, output));
    };
  }

  std::unordered_map<std::string, int> decode(std::array<std::string, 10> input)
  {
    std::array<std::string, 10> results;
    std::unordered_set<std::string> leftovers(input.begin(), input.end());
    for (int i = 0; i < 10; ++i) {
      switch(input[i].size()) {
        case 2:
          // 1 has two segments.
          results[1] = input[i];
          leftovers.erase(input[i]);
          break;
        case 3:
          // 7 has three segments.
          results[7] = input[i];
          leftovers.erase(input[i]);
          break;
        case 4:
          // 4, happily, has four segments.
          results[4] = input[i];
          leftovers.erase(input[i]);
          break;
        case 7:
          // 8 has all 7 segments.
          results[8] = input[i];
          leftovers.erase(input[i]);
          break;
      }
    }
    
    // 3 is the only remaining 5-segment number that has both segments of 1.
    for (auto entry : leftovers) {
      if (entry.size() == 5) {
        std::vector<char> missing;
        std::set_difference(results[1].begin(), results[1].end(), entry.begin(), entry.end(), std::inserter(missing, missing.begin()));
        if (missing.empty()) {
          results[3] = entry;
          leftovers.erase(entry);
          break;
        }
      }
    }
    
    // 9 is the only 6-segment number that has all segments of 4.
    for (auto entry : leftovers) {
      if (entry.size() == 6) {
        std::vector<char> missing;
        std::set_difference(results[4].begin(), results[4].end(), entry.begin(), entry.end(), std::inserter(missing, missing.begin()));
        if (missing.empty()) {
          results[9] = entry;
          leftovers.erase(entry);
          break;
        }
      }
    }
    
    // 0 is the only remaining 6-segment number that has all segments of 7.
    for (auto entry : leftovers) {
      if (entry.size() == 6) {
        std::vector<char> missing;
        std::set_difference(results[7].begin(), results[7].end(), entry.begin(), entry.end(), std::inserter(missing, missing.begin()));
        if (missing.empty()) {
          results[0] = entry;
          leftovers.erase(entry);
          break;
        }
      }
    }
    
    // 6 is the only remaining 6-segment number.
    for (auto entry : leftovers) {
      if (entry.size() == 6) {
        results[6] = entry;
        leftovers.erase(entry);
        break;
      }
    }
    
    // 5 is the only remaining number whose segments are all in 6.
    for (auto entry : leftovers) {
      std::vector<char> remaining_letter;
      std::set_difference(entry.begin(), entry.end(), results[6].begin(), results[6].end(), std::inserter(remaining_letter, remaining_letter.begin()));
      if (remaining_letter.empty()) {
        results[5] = entry;
        leftovers.erase(entry);
        break;
      }
    }
    
    // Last one is 2.
    results[2] = *leftovers.begin();
    
    std::unordered_map<std::string, int> retval;
    for (int i = 0; i < 10; ++i) {
      retval.insert({ results[i], i });
    }

    return retval;
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
  
  long total = 0;
  for (auto entry : digitlist) {
    auto decoded = decode(entry.first);
    long this_number = 0;
    this_number += (1000 * decoded[entry.second[0]]);
    this_number += (100 * decoded[entry.second[1]]);
    this_number += (10 * decoded[entry.second[2]]);
    this_number += decoded[entry.second[3]];
    total += this_number;
  }
  std::cout << "Part Two: " << total << std::endl;

  return 0;
}
