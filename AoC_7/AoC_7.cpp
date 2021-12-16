#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#define REDDIT_SOLUTION 0

namespace
{
  typedef std::vector<int> Crablist;

  void process_file(std::ifstream& input, Crablist& crab)
  {
    char buf[5000];
    input.getline(buf, 5000);
    std::stringstream string_buffer(buf);
    while (!string_buffer.eof()) {
      int n;
      char c;
      string_buffer >> n >> c;
      crab.push_back(n);
    };
  }

#if !REDDIT_SOLUTION
  std::pair<long, long> calculate_fuel(const Crablist& crablist, int position)
  {
    long total_fuel_constant = 0;
    long total_fuel_increase = 0;
    for (auto crab : crablist) {
      unsigned int distance = std::abs(crab - position);
      total_fuel_constant += distance;
      total_fuel_increase += (distance * (distance + 1)) / 2;
    }
    return std::make_pair(total_fuel_constant, total_fuel_increase);
  }
#endif
}

int main()
{
  std::ifstream input("AoC_7_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  Crablist crablist;
  process_file(input, crablist);
  
#if REDDIT_SOLUTION
  // Another fascinating reddit observation. The ideal position for
  // part one is always the median. The ideal position for part two
  // is always the mean.
  std::sort(crablist.begin(), crablist.end());
  int part_1_position = crablist[crablist.size() / 2];
  int part_2_position = std::accumulate(crablist.begin(), crablist.end(), 0l)  / crablist.size();

  long min_fuel_constant = 0;
  long min_fuel_increase = 0;
  for (auto crab : crablist) {
    min_fuel_constant += std::abs(crab - part_1_position);
    unsigned int part_2_distance = std::abs(crab - part_2_position);
    min_fuel_increase += (part_2_distance * (part_2_distance + 1)) / 2;
  }
#else
  long min_fuel_constant = LONG_MAX;
  long min_fuel_increase = LONG_MAX;
  auto minmax_position = std::minmax_element(crablist.begin(), crablist.end());
  for (int i = *minmax_position.first; i != *minmax_position.second; ++i) {
    auto fuel_costs = calculate_fuel(crablist, i);
    min_fuel_constant = std::min(min_fuel_constant, fuel_costs.first);
    min_fuel_increase = std::min(min_fuel_increase, fuel_costs.second);
}
#endif
    
  std::cout << "Part One: " << min_fuel_constant << std::endl;
  std::cout << "Part Two: " << min_fuel_increase << std::endl;

  return 0;
}
