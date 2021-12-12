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

  struct Cave
  {
  public:
    Cave(const std::string& name)
    : name_(name), small_(name[0] >= 'a' && name[0] <= 'z')
    {
      
    }
    
    std::string name_;
    bool small_;
    std::vector<std::weak_ptr<Cave>> connections_;
  };
  typedef std::shared_ptr<Cave> CavePtr;
  typedef std::unordered_map<std::string, CavePtr> Caves;

  void process_file(std::ifstream& input, Caves& caves)
  {
    do {
      char buf[5000];
      input.getline(buf, 5000,'-');
      std::string start = buf;
      input.getline(buf, 5000);
      std::string end = buf;
      auto start_iter = caves.find(start);
      if (start_iter == caves.end()) {
        start_iter = caves.insert( { start, std::make_shared<Cave>(start) }).first;
      }
      auto end_iter = caves.find(end);
      if (end_iter == caves.end()) {
        end_iter = caves.insert( { end, std::make_shared<Cave>(end) }).first;
      }
      start_iter->second->connections_.push_back(end_iter->second);
      end_iter->second->connections_.push_back(start_iter->second);
    }
    while (!input.eof());
  }

  void visit(std::vector<std::vector<CavePtr>>& paths,
             std::vector<CavePtr>&    current,
             std::unordered_multiset<std::string>& visited,
             const std::string& double_visit_cave)
  {
    CavePtr current_cave = current.back();
    if (current_cave->name_ == "end") {
      paths.push_back(current);
    }
    else {
      for (int i = 0; i < current_cave->connections_.size(); ++i) {
        CavePtr next_cave = current_cave->connections_[i].lock();
        size_t visited_count = visited.count(next_cave->name_);
        if (visited_count == 0 || (next_cave->name_ == double_visit_cave && visited_count == 1)) {
          std::unordered_multiset<std::string>::iterator add_iter = visited.end();
          if (next_cave->small_) {
            add_iter = visited.insert(next_cave->name_);
          }
          current.push_back(next_cave);
          visit(paths, current, visited, double_visit_cave);
          current.pop_back();
          if (add_iter != visited.end()) {
            visited.erase(add_iter);
          }
        }
      }
    }
  }
}

int main()
{
  std::ifstream input("AoC_12_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  Caves caves;
  process_file(input, caves);
  
  std::vector<std::vector<CavePtr>> paths;
  std::unordered_multiset<std::string> visited = { "start" };
  std::vector<CavePtr> path = { caves.find("start")->second };
  
  visit(paths, path, visited, "");
  std::cout << "Part One: " << paths.size() << std::endl;
  std::vector<std::vector<CavePtr>> allPaths;
  for (auto iter = caves.begin(); iter != caves.end(); ++iter) {
    paths.clear();
    if (iter->second->small_ && iter->first != "start" && iter->first != "end") {
      std::string caveName = iter->first;
      visit(paths, path, visited, iter->first);
      allPaths.insert(allPaths.end(), paths.begin(), paths.end());
    }
  }
  std::sort(allPaths.begin(), allPaths.end());
  allPaths.erase(std::unique(allPaths.begin(), allPaths.end()), allPaths.end());
  std::cout << "Part Two: " << allPaths.size() << std::endl;
  
  return 0;
}
