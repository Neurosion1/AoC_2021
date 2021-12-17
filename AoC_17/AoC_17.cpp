#include <iostream>
#include <vector>

namespace
{
  int MIN_X = 111;
  int MAX_X = 161;
  int MIN_Y = -154;
  int MAX_Y = -101;

  bool in_target_area(int x, int y)
  {
    return x >= MIN_X && x <= MAX_X && y >= MIN_Y && y <= MAX_Y;
  }

  void move_probe(int & x, int & y, int & dx, int & dy)
  {
    x  += dx;
    y  += dy;
    dx  = std::max(dx - 1, 0);
    dy -= 1;
  }
}

int main(int argc, const char * argv[]) {
  int max_y = -INT_MAX;
  std::vector<std::pair<int, int>> hits;
  // This could probably be better generalized; it currently only works for
  // negative values of { MIN_Y, MAX_Y }. The maximum value of dy has to be
  // abs(MIN_Y) - 1; anything higher than that will result in a probe whose
  // speed is greater than the minimum depth when it returns to zero (so it
  // would shoot right past the target). Note that the minimum starting dx for
  // MY input data is actually 15, but I don't know how to calculate that
  // without applying additional brute force.
  for (int start_dx = 1; start_dx <= MAX_X; ++start_dx) {
    for (int start_dy = -MIN_Y - 1; start_dy >= MIN_Y; --start_dy) {
      int x = 0, y = 0, local_max_y = -INT_MAX;
      int dx = start_dx, dy = start_dy;
      while (x <= MAX_X && y >= MIN_Y) {
        if (dx == 0 && x < MIN_X) {
          break;
        }
        move_probe(x, y, dx, dy);
        local_max_y = std::max(y, local_max_y);
        if (in_target_area(x, y)) {
          max_y = std::max(max_y, local_max_y);
          hits.push_back({ start_dx, start_dy });
          break;
        }
      }
    }
  }
  
  std::cout << "Part One: " << max_y << "\n";
  std::cout << "Part Two: " << hits.size() << "\n";
  return 0;
}
