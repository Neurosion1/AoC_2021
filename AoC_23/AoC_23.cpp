//
//  main.cpp
//  AoC_23
//
//  Created by Neurosion on 12/23/21.
//

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <stack>
#include <array>
#include <list>

namespace
{
  struct Shell
  {
    Shell(char t, int l, int b) : type_(t), location_(l), burrow_(b) {}
    
    char type_;
    int location_;
    int burrow_;
  };
  std::list<Shell> active_shells;

  struct Map
  {
    Map() : hallway_(11, nullptr) {}
    // Legal locations are 0, 1, 3, 5, 7, 9, 10
    std::vector<std::shared_ptr<Shell>> hallway_;
    std::vector<std::stack<std::shared_ptr<Shell>>> burrows_;
  };

  struct Move
  {
    std::shared_ptr<Shell> shell_;
    int old_location_, old_burrow_, new_location_;
  };

  std::vector<Move> get_moves(Map * map, std::shared_ptr<Shell> shell)
  {
    std::vector<Move> retval;
    if (shell->burrow_ == -1) {
      if (map->burrows_[shell->type_ - 'A'].empty()) {
        // Score the piece
      }
      else {
        return retval;
      }
    }
    else {
      if (shell != map->burrows_[shell->burrow_].top()) {
        return retval;
      }
      int burrow_opening = (shell->burrow_  + 1) * 2;
      for (int i = burrow_opening - 1; i >= 0; --i) {
        if (map->hallway_[i] != nullptr) {
          break;
        }
        if ((i % 2) == 1 || i == 0) {
          Move new_move;
          new_move.old_location_ = shell->location_;
          new_move.old_burrow_ = shell->burrow_;
          new_move.new_location_ = i;
        }
      }
      for (int i = burrow_opening + 1; i < map->hallway_.size(); ++i) {
        if (map->hallway_[i] != nullptr) {
          break;
        }
        if ((i % 2) == 1 || i == 10) {
          Move new_move;
          new_move.old_location_ = shell->location_;
          new_move.old_burrow_ = shell->burrow_;
          new_move.new_location_ = i;
        }
      }
    }
    return retval;
  }

  const int burrow_size = 2;
  // NOTE: Items are listed back to front for stack-friendliness.
  char initial_shells[4][burrow_size] = { {'C', 'D'}, {'D', 'C'}, {'A', 'A'}, {'B', 'B'} };
}

int main(int argc, const char * argv[]) {
  Map map;
  for (int i = 0; i < 4; ++i) {
    std::stack<std::shared_ptr<Shell>> this_burrow;
    for (int j = 0; j < burrow_size; ++j) {
      auto this_shell = std::make_shared<Shell>(initial_shells[i][j], j, i);
      this_burrow.push(this_shell);
    }
    map.burrows_.push_back(this_burrow);
  }
  return 0;
}
