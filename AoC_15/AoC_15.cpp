#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
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

  class GraphWalker
  {
  public:
    class Node
    {
    public:
      Node(GraphWalker * parent, int cost) : parent_(parent), cost_(cost), path_cost_(INT_MAX), visited_(false) {}
      
      void initialize(int x, int y)
      {
        if (x > 0) {
          connections_.push_back(&parent_->nodeGrid_[x-1][y]);
        }
        if (x < parent_->nodeGrid_.size() - 1) {
          connections_.push_back(&parent_->nodeGrid_[x+1][y]);
        }
        if (y > 0) {
          connections_.push_back(&parent_->nodeGrid_[x][y-1]);
        }
        if (y < parent_->nodeGrid_[0].size() - 1) {
          connections_.push_back(&parent_->nodeGrid_[x][y+1]);
        }
      }
      
      void touch(int incoming_cost)
      {
        if (cost_ + incoming_cost < path_cost_) {
          path_cost_ = cost_ + incoming_cost;
          auto unvisited_iter = parent_->unvisited_.find(this);
          if (unvisited_iter != parent_->unvisited_.end()) {
            parent_->unvisited_costs_.erase(unvisited_iter->second);
          }
          auto new_cost_iter = parent_->unvisited_costs_.insert( { path_cost_, this } );
          parent_->unvisited_.insert( { this, new_cost_iter } );
        }
      }
      
      void visit()
      {
        for (int i = 0; i < connections_.size(); ++i) {
          if (!connections_[i]->visited_) {
            connections_[i]->touch(path_cost_);
          }
        }
        visited_ = true;
      }
      
      int get_path_cost() const
      {
        return path_cost_;
      }
      
      void set_path_cost(int n)
      {
        path_cost_ = n;
      }
      
    private:
      int cost_;
      int path_cost_;
      bool visited_;
      GraphWalker * parent_;
      std::vector<Node *> connections_;
    };
    typedef std::vector<Node> NodeRow;
    typedef std::vector<NodeRow> NodeGrid;
    
    struct CompareNodeCosts
    {
      bool operator()(const Node * left, const Node * right) { return left->get_path_cost() < right->get_path_cost(); }
    };
    
    GraphWalker(const Grid& costGrid)
    {
      for (int x = 0; x < costGrid.size(); ++x) {
        NodeRow row;
        for (int y = 0; y < costGrid[0].size(); ++y) {
          row.push_back(Node(this, costGrid[x][y]));
        }
        nodeGrid_.push_back(row);
      }
      nodeGrid_[0][0].set_path_cost(0);
      
      for (int x = 0; x < nodeGrid_.size(); ++x) {
        for (int y = 0; y < nodeGrid_.size(); ++y) {
          nodeGrid_[x][y].initialize(x, y);
        }
      }
      auto iter = unvisited_costs_.insert( { nodeGrid_[0][0].get_path_cost(), &nodeGrid_[0][0] } );
      unvisited_.insert( { &nodeGrid_[0][0], iter } );
    }
    
    int get_minimum_path_length()
    {
      while (!unvisited_.empty()) {
        auto iter = unvisited_costs_.begin();
        Node * node = iter->second;
        unvisited_.erase(node);
        unvisited_costs_.erase(iter);
        node->visit();
      }
      
      return nodeGrid_[nodeGrid_.size()-1][nodeGrid_[0].size()-1].get_path_cost();
    }
    
  private:
    NodeGrid nodeGrid_;
    std::multimap<int, Node *> unvisited_costs_;
    std::unordered_map<Node *, std::multimap<int, Node *>::iterator> unvisited_;
  };
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
  
  GraphWalker walker(grid);
  std::cout << "Part One: " << walker.get_minimum_path_length() << "\n";
  
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
  
  GraphWalker nextWalker(finalGrid);
  std::cout << "Part Two: " << nextWalker.get_minimum_path_length() << std::endl;
  
  return 0;
}
