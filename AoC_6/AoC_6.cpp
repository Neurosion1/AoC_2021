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
  typedef std::list<int> Fishlist;

  void process_file(std::ifstream& input, Fishlist& fish)
  {
    char buf[5000];
    input.getline(buf, 5000);
    std::stringstream string_buffer(buf);
    while (!string_buffer.eof()) {
      int n;
      char c;
      string_buffer >> n >> c;
      fish.push_back(n);
    };
  }

  size_t modelFishAfterDays(Fishlist fishlist, int days)
  {
    for (int day = 0; day < days; ++day) {
      int newFish = 0;
      for (auto & fish : fishlist) {
        if (fish == 0) {
          ++newFish;
          fish = 6;
        }
        else {
          --fish;
        }
      }
      for (int newFishIndex = 0; newFishIndex < newFish; ++newFishIndex) {
        fishlist.push_back(8);
      }
    }
    
    return fishlist.size();
  }

#if 0
  long long mathFishAfterDays(Fishlist fishlist, int days)
  {
    std::array<long long, 9> fishcount = {};
    for (auto fish : fishlist) {
      ++fishcount[fish];
    }
    
    for (int day = 0; day < days; ++day) {
      long long zero_fish = fishcount[0];
      for (int i = 0; i < 8; ++i) {
        fishcount[i] = fishcount[i + 1];
      }
      fishcount[6] += zero_fish;
      fishcount[8] = zero_fish;
    }
    
    long long answer = 0;
    for (int i = 0; i < 9; ++i) {
      answer += fishcount[i];
    }
    
    return answer;
  }
}
#else
  // NOTE: Saw someone mention std::rotate<>() on reddit, which inspired
  //       this lovely solution.
  long long mathFishAfterDays(Fishlist fishlist, int days)
  {
    std::array<long long, 9> fishcount = {};
    std::for_each(fishlist.begin(), fishlist.end(), [&fishcount](auto fish) { ++fishcount[fish]; });

    for (int day = 0; day < days; ++day) {
      std::rotate(fishcount.begin(), fishcount.begin() + 1, fishcount.end());
      fishcount[6] += fishcount[8];
    }

    return std::accumulate(fishcount.begin(), fishcount.end(), 0ll);
  }
#endif

int main()
{
  std::ifstream input("AoC_6_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  Fishlist fishlist;
  process_file(input, fishlist);
  
  std::cout << "Part One: " << modelFishAfterDays(fishlist, 80)  << std::endl;
  std::cout << "Part Two: " << mathFishAfterDays (fishlist, 256) << std::endl;
  
  return 0;
}
