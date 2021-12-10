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
#include <map>
#include <queue>

namespace {
  void process_file(std::ifstream& input, std::list<std::string>& lines)
  {
    while (!input.eof()) {
      char buf[5000];
      input.getline(buf, 5000);
      lines.emplace_back(std::string(buf));
    };
  }

  bool is_open_char(char c)
  {
    switch(c)
    {
      case '(':
      case '{':
      case '[':
      case '<':
        return true;
    }
    return false;
  }

  char get_matching_char(char o)
  {
    switch(o)
    {
      case '(': return ')';
      case '{': return '}';
      case '[': return ']';
      case '<': return '>';
    }
    return '?';
  }

  int get_corrupted_score(char c)
  {
    switch(c)
    {
      case ')': return 3;
      case ']': return 57;
      case '}': return 1197;
      case '>': return 25137;
    }
    return 0;
  }

  int get_completed_score(char c)
  {
    switch(c)
    {
      case ')': return 1;
      case ']': return 2;
      case '}': return 3;
      case '>': return 4;
    }
    return 0;
  }
}

int main()
{
  std::ifstream input("AoC_10_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  std::list<std::string> lines;
  process_file(input, lines);
  
  int corrupted_score = 0;
  std::vector<long long> completion_scores;
  for (auto line : lines) {
    std::stack<char> opens;
    bool is_corrupted = false;
    for (auto c : line) {
      if (is_open_char(c)) {
        opens.push(c);
      }
      else {
        if (c == get_matching_char(opens.top())) {
          opens.pop();
        }
        else {
          is_corrupted = true;
          corrupted_score += get_corrupted_score(c);
          break;
        }
      }
    }
    if (!is_corrupted && !opens.empty()) {
      long long completion_score = 0;
      while (!opens.empty()) {
        completion_score *= 5;
        completion_score += get_completed_score(get_matching_char(opens.top()));
        opens.pop();
      }
      completion_scores.push_back(completion_score);
    }
  }
  std::sort(completion_scores.begin(), completion_scores.end());
  
  std::cout << "Part One: " << corrupted_score << std::endl;
  std::cout << "Part Two: " << completion_scores[completion_scores.size() / 2] << std::endl;
  
  return 0;
}
