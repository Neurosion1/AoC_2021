#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

namespace
{
  enum class Direction
  {
    eForward,
    eBackward,
    eUp,
    eDown
  };

  std::unordered_map<std::string, Direction> direction_map =
  { {"forward", Direction::eForward},
    {"backward", Direction::eBackward},
    {"up", Direction::eUp},
    {"down", Direction::eDown} };

  std::pair<Direction, int> get_next(std::ifstream& input)
  {
    static std::string direction;
    static int distance;
    input >> direction;
    input >> distance;
    return { direction_map[direction], distance };
  }

  int AoC_2_1(std::ifstream& input)
  {
    int x = 0, y = 0;
    do {
      const auto & next = get_next(input);
      switch (next.first)
      {
      case Direction::eForward:
        x += next.second;
        break;
      case Direction::eDown:
        y += next.second;
        break;
      case Direction::eUp:
        y -= next.second;
        break;
      }
    } while (!input.eof());

    return x * y;
  }

  int AoC_2_2(std::ifstream& input)
  {
    int aim = 0, x = 0, y = 0;
    do {
      const auto & next = get_next(input);
      switch (next.first)
      {
      case Direction::eForward:
        x += next.second;
        y += (aim * next.second);
        break;
      case Direction::eDown:
        aim += next.second;
        break;
      case Direction::eUp:
        aim -= next.second;
        break;
      }
    } while (!input.eof());

    return x * y;
  }
}

int main()
{
  std::ifstream input("AoC_2_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }

  std::cout << AoC_2_2(input) << std::endl;

  return 0;
}
