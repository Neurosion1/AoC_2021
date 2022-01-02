#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <list>

namespace
{
  struct Range
  {
    long long start_x_, end_x_;
    long long start_y_, end_y_;
    long long start_z_, end_z_;
  };

  class Engine
  {
  public:
    void subtract_segment(const Range& subtraction)
    {
      std::list<Range> new_segments;
      for (auto segment = segments_.begin(); segment != segments_.end();) {
        if (segment->start_x_ <= subtraction.end_x_   &&
            segment->end_x_   >= subtraction.start_x_ &&
            segment->start_y_ <= subtraction.end_y_   &&
            segment->end_y_   >= subtraction.start_y_ &&
            segment->start_z_ <= subtraction.end_z_   &&
            segment->end_z_   >= subtraction.start_z_) {
          if (segment->start_x_ < subtraction.start_x_) {
            Range x_slice_1 = Range {
              segment->start_x_, subtraction.start_x_ - 1,
              segment->start_y_, segment->end_y_,
              segment->start_z_, segment->end_z_ };
            segment->start_x_ = subtraction.start_x_;
            new_segments.push_back(x_slice_1);
          }
          if (segment->end_x_ > subtraction.end_x_) {
            Range x_slice_2 = Range {
              subtraction.end_x_ + 1, segment->end_x_,
              segment->start_y_, segment->end_y_,
              segment->start_z_, segment->end_z_ };
            segment->end_x_ = subtraction.end_x_;
            new_segments.push_back(x_slice_2);
          }
          if (segment->start_y_ < subtraction.start_y_) {
            Range y_slice_1 = Range {
              segment->start_x_, segment->end_x_,
              segment->start_y_, subtraction.start_y_ - 1,
              segment->start_z_, segment->end_z_ };
            segment->start_y_ = subtraction.start_y_;
            new_segments.push_back(y_slice_1);
          }
          if (segment->end_y_ > subtraction.end_y_) {
            Range y_slice_2 = Range {
              segment->start_x_, segment->end_x_,
              subtraction.end_y_ + 1, segment->end_y_,
              segment->start_z_, segment->end_z_ };
            segment->end_y_ = subtraction.end_y_;
            new_segments.push_back(y_slice_2);
          }
          if (segment->start_z_ < subtraction.start_z_) {
            Range z_slice_1 = Range {
              segment->start_x_, segment->end_x_,
              segment->start_y_, segment->end_y_,
              segment->start_z_, subtraction.start_z_ - 1 };
            segment->start_z_ = subtraction.start_z_;
            new_segments.push_back(z_slice_1);
          }
          if (segment->end_z_ > subtraction.end_z_) {
            Range z_slice_2 = Range {
              segment->start_x_, segment->end_x_,
              segment->start_y_, segment->end_y_,
              subtraction.end_z_ + 1, segment->end_z_ };
            segment->end_z_ = subtraction.end_z_;
            new_segments.push_back(z_slice_2);
          }
          segments_.erase(segment++);
        }
        else {
          ++segment;
        }
      }
      segments_.splice(segments_.end(), new_segments);
    }
    
    void add_segment(const Range& addition)
    {
      subtract_segment(addition);
      segments_.push_back(addition);
    }
    
    long long power()
    {
      return std::accumulate(segments_.begin(), segments_.end(), 0ll,
                             [](long long total, const Range& item)
                             {
                                return total + (item.end_x_ - item.start_x_ + 1) *
                                               (item.end_y_ - item.start_y_ + 1) *
                                               (item.end_z_ - item.start_z_ + 1);
                             }
      );
    }
    
  private:
    std::list<Range> segments_;
  };
}

int main()
{
  std::ifstream input("AoC_22_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  int x = 0;
  Engine engine;
  do {
    char buf[5000];
    input.getline(buf, 5000);
    std::stringstream string_input(buf);
    std::string on_off;
    char xc, yc, zc, equals, dot, comma;
    Range this_range;
    string_input >> on_off >>
                    xc >> equals >> this_range.start_x_ >> dot >> dot >> this_range.end_x_ >> comma >>
                    yc >> equals >> this_range.start_y_ >> dot >> dot >> this_range.end_y_ >> comma >>
                    zc >> equals >> this_range.start_z_ >> dot >> dot >> this_range.end_z_;
    if (on_off == "on") {
      engine.add_segment(this_range);
    }
    else {
      engine.subtract_segment(this_range);
    }
    if (++x == 20) {
      std::cout << "Part One: " << engine.power() << "\n";
    }
  } while (!input.eof());
  
  std::cout << "Part Two: " << engine.power() << "\n";
  
  return 0;
}
