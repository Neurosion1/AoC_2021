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
      input.getline(buf, 5000, '-');
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

  std::string string_path(const std::vector<CavePtr>& path)
  {
    std::string retval;
    // Skip "start" and "end"
    for (int i = 1; i < path.size() - 1; ++i) {
      retval += path[i]->name_;
    }
    return retval;
  }

  void visit(std::unordered_set<std::string>& paths,
             std::vector<CavePtr>&    current,
             std::unordered_multiset<CavePtr>& visited,
             const CavePtr& end_cave,
             const CavePtr& double_visit_cave)
  {
    CavePtr current_cave = current.back();
    for (int i = 0; i < current_cave->connections_.size(); ++i) {
      CavePtr next_cave = current_cave->connections_[i].lock();
      if (current_cave == end_cave) {
        paths.emplace(string_path(current));
      }
      else {
        size_t visited_count = visited.count(next_cave);
        // Crap like the following is your hint that it's time to stop noodling.
        if (visited_count <= static_cast<int>(next_cave == double_visit_cave)) {
          std::unordered_multiset<CavePtr>::iterator add_iter = visited.end();
          if (next_cave->small_) {
            add_iter = visited.insert(next_cave);
          }
          current.push_back(next_cave);
          visit(paths, current, visited, end_cave, double_visit_cave);
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
  
  CavePtr startCave = caves["start"], endCave = caves["end"];
  std::unordered_set<std::string> paths;
  std::unordered_multiset<CavePtr> visited = { startCave };
  std::vector<CavePtr> path = { startCave };
  
  visit(paths, path, visited, endCave, nullptr);
  std::cout << "Part One: " << paths.size() << std::endl;
  paths.clear();
  for (auto iter = caves.begin(); iter != caves.end(); ++iter) {
    if (iter->second->small_ && iter->second != startCave && iter->second != endCave) {
      visit(paths, path, visited, endCave, iter->second);
    }
  }
  std::cout << "Part Two: " << paths.size() << std::endl;
  
  return 0;
}
