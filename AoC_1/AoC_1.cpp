#include <iostream>
#include <fstream>
#include <queue>

namespace
{
  int get_next(std::ifstream& input)
  {
    static int retval;
    input >> retval;
    return retval;
  }

  int AoC_1_1(std::ifstream& input)
  {
    int previous = 0, current = 0, count = 0;
    input >> previous;
    do {
      current = get_next(input);
      if (current > previous) {
        ++count;
      }
      previous = current;
    } while (!input.eof());

    return count;
  }

  int AoC_1_2(std::ifstream& input)
  {
    static const int WINDOW_SIZE = 3;
    std::queue<int> window;
    for (int i = 0; i < WINDOW_SIZE; ++i) {
      window.push(get_next(input));
    }

    int count = 0;
    do {
      int next = get_next(input);
      if (next > window.front()) {
        ++count;
      }
      window.pop();
      window.push(next);
    } while (!input.eof());

    return count;
  }
}

int main()
{
  std::ifstream input("AoC_1_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }

  std::cout << AoC_1_2(input) << std::endl;

  return 0;
}
