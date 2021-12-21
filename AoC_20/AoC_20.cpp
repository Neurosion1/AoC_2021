#include <iostream>
#include <fstream>
#include <deque>
#include <numeric>

namespace
{
  void process_file(std::ifstream& input, std::string& enhancement, std::deque<std::string>& image)
  {
    char buf[5000];
    input.getline(buf, 5000);
    enhancement = buf;
    input.getline(buf, 5000);
    do {
      input.getline(buf, 5000);
      std::string line(buf);
      image.push_back(line);
    } while (!input.eof());
  }

  void expand_image(std::deque<std::string>& image, const std::string& enhancement, int iteration)
  {
    // We make the assumption that when enhancement[0] (all dark) translates to
    // light, enhancement[511] (all light) MUST translate to dark. If this were
    // not the case, the answer to "count the lit pixels" would be infinity for
    // any iteration beyond the first.
    static const std::string two_fill[2] = { std::string(2, '.'), std::string(2, '#') };
    char fill = (enhancement[0] == '#' && iteration % 2 == 1) ? '#' : '.';
    for (int i = 0; i < image.size(); ++i) {
      image[i] = two_fill[fill == '#'] + image[i] + two_fill[fill == '#'];
    }
    image.push_front(std::string(image[0].size(), fill));
    image.push_front(std::string(image[0].size(), fill));
    image.push_back(std::string(image[0].size(), fill));
    image.push_back(std::string(image[0].size(), fill));
  }

  std::deque<std::string> enhance_image(const std::deque<std::string>& input, const std::string& enhancement)
  {
    std::deque<std::string> retval;
    const int transformations[9][2] = { {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1} };
    for (int i = 1; i < input.size() - 1; ++i) {
      std::string output_line;
      for (int j = 1; j < input[0].size() - 1; ++j) {
        int index = 0;
        for (int k = 0; k < 9; ++k) {
          if (input[transformations[k][0] + i][transformations[k][1] + j] == '#') {
            index |= 1 << (8 - k);
          }
        }
        output_line += enhancement[index];
      }
      retval.push_back(output_line);
    }
    return retval;
  }

  int count_pixels(const std::deque<std::string>& image)
  {
    return std::accumulate(image.begin(), image.end(), 0, [] (int current, const std::string& s) {
      return current + std::count(s.begin(), s.end(), '#');
    });
  }
}

int main()
{
  std::ifstream input("AoC_20_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  std::string enhancement;
  std::deque<std::string> image;
  process_file(input, enhancement, image);
  
  int i = 0;
  for (; i < 2; ++i) {
    expand_image(image, enhancement, i);
    image = enhance_image(image, enhancement);
  }
  
  std::cout << "Part One: " << count_pixels(image) << "\n";
  
  for (; i < 50; ++i) {
    expand_image(image, enhancement, i);
    image = enhance_image(image, enhancement);
  }
  
  std::cout << "Part Two: " << count_pixels(image) << "\n";
  
  return 0;
}
