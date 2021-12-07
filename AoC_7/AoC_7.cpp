#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <numeric>

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

  std::pair<long, long> calculate_fuel(const Crablist& crablist, int position)
  {
    long total_fuel_constant = 0;
    long total_fuel_increase = 0;
    for (int i = 0; i < crablist.size(); ++i) {
      unsigned int distance = std::abs(crablist[i] - position);
      total_fuel_constant += distance;
      total_fuel_increase += (distance * (distance + 1)) / 2;
    }
    return std::make_pair(total_fuel_constant, total_fuel_increase);
  }
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
  
  int min_position = *std::min_element(crablist.begin(), crablist.end());
  int max_position = *std::max_element(crablist.begin(), crablist.end());
  
  long min_fuel_constant = LONG_MAX;
  long min_fuel_increase = LONG_MAX;
  for (int i = min_position; i != max_position; ++i) {
    auto fuel_costs = calculate_fuel(crablist, i);
    min_fuel_constant = std::min(min_fuel_constant, fuel_costs.first);
    min_fuel_increase = std::min(min_fuel_increase, fuel_costs.second);
  }
  
  std::cout << "Part One: " << min_fuel_constant << std::endl;
  std::cout << "Part Two: " << min_fuel_increase << std::endl;
  
  return 0;
}
