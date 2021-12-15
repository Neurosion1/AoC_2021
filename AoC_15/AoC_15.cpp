#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
#include <vector>
#include <list>
#include <unordered_map>

namespace
{
typedef std::vector<int> GridRow;
typedef std::vector<GridRow> Grid;

  void process_file(std::ifstream& input, Grid& grid)
  {
    do {
      char buf[5000];
      input.getline(buf, 5000);
      std::vector<int> row;
      std::stringstream row_str(buf);
      while (!row_str.eof()) {
        char c;
        row_str >> c;
        if (!row_str.eof()) {
          row.push_back(c - '0');
        }
      }
      grid.push_back(row);
    } while (!input.eof());
  }

#if 0
  // Algorithm that only works if every step of the path goes down or to the
  // right. Unfortunately worked perfectly for Part One with my data and for
  // both parts of the test data. I doubt that was an accident.
  int get_minimum_path_dumb(const Grid& grid)
  {
    GridRow totalGridRow(grid[0].size(), 0);
    Grid totalGrid(grid.size(), totalGridRow);
    for (int x = 1; x < grid.size(); ++x) {
      totalGrid[x][0] = grid[x][0] + totalGrid[x-1][0];
    }
    for (int y = 1; y < grid[0].size(); ++y) {
      totalGrid[0][y] = grid[0][y] + totalGrid[0][y-1];
    }
    for (int x = 1; x < grid.size(); ++x) {
      for (int y = 1; y < grid[0].size(); ++y) {
        totalGrid[x][y] = grid[x][y] + std::min(totalGrid[x-1][y], totalGrid[x][y-1]);
      }
    }
    
    return totalGrid[grid.size()-1][grid[0].size()-1];
  }
#endif

struct Node;
  typedef std::vector<Node> NodeRow;
  typedef std::vector<NodeRow> NodeGrid;
  struct Node
  {
    Node() : path_cost_(INT_MAX), visited_(false) {}
    
    void initialize(NodeGrid& nodeGrid, const Grid& costGrid, std::list<Node *> * unvisited, int x, int y)
    {
      cost_ = costGrid[x][y];
      unvisited_ = unvisited;
      if (x > 0) {
        connections_.push_back(&nodeGrid[x-1][y]);
      }
      if (x < costGrid.size() - 1) {
        connections_.push_back(&nodeGrid[x+1][y]);
      }
      if (y > 0) {
        connections_.push_back(&nodeGrid[x][y-1]);
      }
      if (y < costGrid[0].size() - 1) {
        connections_.push_back(&nodeGrid[x][y+1]);
      }
    }
    
    void update_cost(int incoming_cost)
    {
      if (path_cost_ == INT_MAX) {
        unvisited_->push_back(this);
      }
      if (cost_ + incoming_cost < path_cost_) {
        path_cost_ = cost_ + incoming_cost;
      }
    }
    
    void visit()
    {
      for (int i = 0; i < connections_.size(); ++i) {
        if (!connections_[i]->visited_) {
          connections_[i]->update_cost(path_cost_);
        }
      }
      visited_ = true;
    }
    
    int cost_;
    int path_cost_;
    bool visited_;
    std::vector<Node *> connections_;
    std::list<Node *> * unvisited_;
  };

  

  int get_minimum_path_walk(const Grid& grid)
  {
    NodeRow nodeRow(grid[0].size());
    NodeGrid nodeGrid(grid.size(), nodeRow);
    nodeGrid[0][0].path_cost_ = 0;
    
    std::list<Node *> unvisited;
    unvisited.push_back(&nodeGrid[0][0]);
    for (int x = 0; x < nodeGrid.size(); ++x) {
      for (int y = 0; y < nodeGrid.size(); ++y) {
        nodeGrid[x][y].initialize(nodeGrid, grid, &unvisited, x, y);
      }
    }
    
    auto next_visit = unvisited.begin();
    do {
      (*next_visit)->visit();
      unvisited.erase(next_visit);
      int min_path_cost = INT_MAX;
      if (unvisited.empty()) {
        break;
      }
      for (auto iter = unvisited.begin(); iter != unvisited.end(); ++iter) {
        if ((*iter)->path_cost_ < min_path_cost) {
          next_visit = iter;
          min_path_cost = (*iter)->path_cost_;
        }
      }
    }
    while (true);
    
    return nodeGrid[grid.size()-1][grid[0].size()-1].path_cost_;
  }
}

int main()
{
  std::ifstream input("AoC_15_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  Grid grid;
  process_file(input, grid);
  
  std::cout << "Part One: " << get_minimum_path_walk(grid) << "\n";
  
  GridRow finalGridRow(grid[0].size() * 5, 0);
  Grid finalGrid(grid.size() * 5, finalGridRow);
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      for (int x = 0; x < grid.size(); ++x) {
        for (int y = 0; y < grid[0].size(); ++y) {
          size_t target_x = i * grid.size() + x;
          size_t target_y = j * grid.size() + y;
          int risk;
          if (i > 0) {
            risk = finalGrid[target_x - grid.size()][target_y] % 9 + 1;
          }
          else if (j > 0) {
            risk = finalGrid[target_x][target_y - grid.size()] % 9 + 1;
          }
          else {
            risk = grid[x][y];
          }
          finalGrid[target_x][target_y] = risk;
        }
      }
    }
  }
  
  std::cout << "Part Two: " << get_minimum_path_walk(finalGrid) << std::endl;
  
  return 0;
}
