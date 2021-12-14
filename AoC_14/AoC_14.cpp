#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <numeric>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace {
  typedef std::unordered_map<std::string, char> PolymerMap;

  void process_file(std::ifstream& input, std::list<char>& current, PolymerMap& map)
  {
    std::string current_string;
    input >> current_string;
    for (char c : current_string) {
      current.push_back(c);
    }
    do {
      char buf[5000];
      input.getline(buf, 5000);
      if (strlen(buf) > 0) {
        std::stringstream line(buf);
        std::string pair, arrow;
        char c;
        line >> pair >> arrow >> c;
        map.insert( { pair, c} );
      }
    } while (!input.eof());
  }

  void apply_polymers(std::list<char>& current, PolymerMap& map)
  {
    auto front_iter = current.begin(), back_iter = front_iter;
    ++front_iter;
    char lookup[3] = { 0 };
    while (front_iter != current.end()) {
      lookup [0] = *back_iter;
      lookup [1] = *front_iter;
      auto iter = map.find(lookup);
      if (iter != map.end()) {
        ++back_iter;
        current.insert(front_iter++, iter->second);
      }
    }
  }
}

int main()
{
  std::ifstream input("AoC_14_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  std::list<char> current;
  PolymerMap map;
  process_file(input, current, map);
  
  for (int i = 0; i < 10; ++i) {
    apply_polymers(current, map);
  }
  
  std::unordered_multiset<char> counter;
  for (auto c : current) {
    counter.insert(c);
  }
  
  long min = INT_MAX, max = 0;
  for (auto item : counter) {
    long item_count = counter.count(item);
    min = std::min(min, item_count);
    max = std::max(max, item_count);
  }
  
  std::cout << "Part One: " << max - min << std::endl;
  
  //std::cout << "Part Two: " << "\n\n";
  
  return 0;
}
