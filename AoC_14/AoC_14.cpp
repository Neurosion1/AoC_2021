#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
#include <unordered_map>

namespace
{
  struct PolymerMachine
  {
    typedef std::unordered_map<std::string, char> PolymerMap;
    typedef std::unordered_map<std::string, long long> PairMap;
    typedef std::unordered_map<char, long long> ElementMap;

    PolymerMachine(std::string start, const std::unordered_map<std::string, char>& map)
      : polymer_map_(map)
    {
      char item[3] = { 0 };
      for (size_t i = 0; i < start.size() - 1; ++i)
      {
        item[0] = start[i];
        item[1] = start[i + 1];
        ++pair_map_[item];
        ++element_map_[item[0]];
      }
      // Don't forget to add the last element.
      ++element_map_[item[1]];
    }

    void apply()
    {
      PairMap new_map = pair_map_;
      char lookup[3] = { 0 };
      for (auto item : pair_map_) {
        auto iter = polymer_map_.find(item.first);
        if (iter != polymer_map_.end()) {
          // Add a new element for every old pair
          element_map_[iter->second] += item.second;

          // Add new pair { front old pair element, new element } for every old pair
          lookup[0] = item.first[0];
          lookup[1] = iter->second;
          new_map[lookup] += item.second;

          // Add new pair { new element,  back old pair element } for every old pair
          lookup[0] = iter->second;
          lookup[1] = item.first[1];
          new_map[lookup] += item.second;

          // Remove every old pair
          new_map[item.first] -= item.second;
        }
      }
      pair_map_ = new_map;
    }

    long long get_min_max_diff()
    {
      long long min = LLONG_MAX, max = 0;
      for (auto& item : element_map_) {
        min = std::min(min, item.second);
        max = std::max(max, item.second);
      }
      return max - min;
    }

    PairMap pair_map_;
    ElementMap element_map_;
    const PolymerMap polymer_map_;
  };

  PolymerMachine process_file(std::ifstream& input)
  {
    PolymerMachine::PolymerMap map;
    std::string current_string;
    input >> current_string;
    do {
      char buf[5000];
      input.getline(buf, 5000);
      if (strlen(buf) > 0) {
        std::stringstream line(buf);
        std::string pair, arrow;
        char c;
        line >> pair >> arrow >> c;

        map.insert({ pair, c });
      }
    } while (!input.eof());

    return PolymerMachine(current_string, map);
  }
}

int main()
{
  std::ifstream input("AoC_14_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }

  PolymerMachine machine = process_file(input);

  int i = 0;

  for (; i < 10; ++i) { machine.apply(); }
  std::cout << "Part One: " << machine.get_min_max_diff() << "\n";

  for (; i < 40; ++i) { machine.apply(); }
  std::cout << "Part Two: " << machine.get_min_max_diff() << std::endl;

  return 0;
}
