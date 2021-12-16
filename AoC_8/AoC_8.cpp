#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>

namespace
{
  typedef std::array<std::string, 10> Signal;
  typedef std::array<std::string, 4>  Output;
  typedef std::vector<std::pair<Signal, Output>> Digitlist;

  void process_file(std::ifstream& input, Digitlist& digitlist)
  {
    std::string delimiter;
    while (!input.eof()) {
      char buf[5000];
      input.getline(buf, 5000);
      std::stringstream string_buffer(buf);
      Signal signal;
      Output output;
      for (int i = 0; i < 10; ++i) {
        std::string digits;
        string_buffer >> digits;
        std::sort(digits.begin(), digits.end());
        signal[i] = digits;
      }
      string_buffer >> delimiter;
      for (int i = 0; i < 4; ++i) {
        std::string digits;
        string_buffer >> digits;
        std::sort(digits.begin(), digits.end());
        output[i] = digits;
      }
      digitlist.emplace_back(std::make_pair(signal, output));
    };
  }

  // Assumes both strings are sorted.
  bool is_subset(const std::string& test_range, const std::string& source_range)
  {
    for (auto source_iter = source_range.cbegin(), test_iter = test_range.cbegin();
              test_iter != test_range.cend();
            ++test_iter)
    {
      while (*source_iter != *test_iter) {
        if (++source_iter == source_range.cend()) {
          return false;
        }
      }
    }
    return true;
  }

  std::unordered_map<std::string, int> decode(std::array<std::string, 10> input)
  {
    std::array<std::string, 10> results;
    std::unordered_set<std::string> fives;
    std::unordered_set<std::string> sixes;
    for (int i = 0; i < 10; ++i) {
      switch(input[i].size()) {
        case 2:
          // 1 has two segments.
          results[1] = input[i];
          break;
        case 3:
          // 7 has three segments.
          results[7] = input[i];
          break;
        case 4:
          // 4, happily, has four segments.
          results[4] = input[i];
          break;
        case 5:
          fives.insert(input[i]);
          break;
        case 6:
          sixes.insert(input[i]);
          break;
        case 7:
          // 8 has all 7 segments.
          results[8] = input[i];
          break;
      }
    }

    // 9 is the only 6-segment number that has all segments of 4.
    for (const auto & entry : sixes) {
      if (is_subset(results[4], entry)) {
        results[9] = entry;
        sixes.erase(entry);
        break;
      }
    }
    
    // 0 is the only remaining 6-segment number that has all segments of 7.
    for (const auto & entry : sixes) {
      if (is_subset(results[7], entry)) {
        results[0] = entry;
        sixes.erase(entry);
        break;
      }
    }
    
    // 6 is the only remaining 6-segment number.
    results[6] = *sixes.begin();

    // 3 is the only 5-segment number that has both segments of 1.
    for (const auto & entry : fives) {
      if (is_subset(results[1], entry)) {
        results[3] = entry;
        fives.erase(entry);
        break;
      }
    }
    
    // 5 is the only 5-segment number whose segments are all in 6.
    for (const auto & entry : fives) {
      if (is_subset(entry, results[6])) {
        results[5] = entry;
        fives.erase(entry);
        break;
      }
    }
    
    // Last is 2.
    results[2] = *fives.begin();
    
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

  long part_one_count = 0;
  long part_two_count = 0;
  for (auto & entry : digitlist) {
    long multiplier = 1000;
    auto decoded = decode(entry.first);
    for (int i = 0; i < 4; ++i) {
      long digit = decoded[entry.second[i]];
      switch (digit) {
      case 1:
      case 4:
      case 7:
      case 8:
        ++part_one_count;
      }
      part_two_count += multiplier * digit;
      multiplier /= 10;
    }
  }

  std::cout << "Part One: " << part_one_count << std::endl;
  std::cout << "Part Two: " << part_two_count << std::endl;

  return 0;
}
