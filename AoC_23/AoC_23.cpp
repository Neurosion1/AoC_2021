#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <deque>
#include <array>

#ifdef _MSC_VER
#include <assert.h>
#endif

namespace
{
  struct Shell;
  
  struct Board
  {
    Board(int burrow_depth) : burrow_depth_(burrow_depth), hallway_(11, '.'), burrows_(4, std::string(burrow_depth, '.')), burrows_filled_(4, -1) {}
    int burrow_depth_;
    std::string hallway_;
    std::vector<std::string> burrows_;
    std::vector<int> burrows_filled_; // Starts at -1 (unavailable), then becomes count of shells in burrow.
    std::vector<Shell> available_shells_;
  };
  
  enum class Location
  {
    eHallway = -1,
    eBurrowA,
    eBurrowB,
    eBurrowC,
    eBurrowD
  };
  
  struct Shell
  {
    char type_;
    Location location_;
    int position_;
    int get_cost() const
    {
      switch (type_)
      {
        case 'A':
          return 1;
        case 'B':
          return 10;
        case 'C':
          return 100;
        case 'D':
          return 1000;
      }
      assert(false);
      return -1;
    }
  };
  
  int type_to_target_burrow(const Shell& shell)
  {
    return shell.type_ - 'A';
  }
  
  int get_target_burrow_location(const Shell& shell)
  {
    return (2 * (shell.type_ - 'A')) + 2;
  }
  
  struct Move
  {
    Move (Location location, int position, int distance) : location_(location), position_(position), distance_(distance) {}
    Location location_;
    int position_;
    int distance_;
  };
  
  std::string get_board_state(const Board& board)
  {
    std::string retval;
    for (char c : board.hallway_) {
      retval.push_back(c);
    }
    for (int i = 0; i < board.burrows_.size(); ++i) {
      for (int j = 0; j < board.burrow_depth_; ++j) {
        retval.push_back(board.burrows_[i][j]);
      }
    }
    return retval;
  }
  
  void dump(const Board& board)
  {
    std::cout << "#############\n#";
    for (char c : board.hallway_) {
      std::cout << c;
    }
    std::cout << "#\n##";
    for (auto burrow : board.burrows_) {
      std::cout << "#" << burrow[0];
    }
    std::cout << "###\n";
    for (int i = 1; i < board.burrow_depth_; ++i) {
      std::cout << "  ";
      for (auto burrow : board.burrows_) {
        std::cout << "#" << burrow[i];
      }
      std::cout << "#\n";
    }
    std::cout << "  #########\n\n";
  }
  
  std::vector<Move> get_moves(const Board& board, const Shell& shell)
  {
    std::vector<Move> retval;
    int target_burrow = type_to_target_burrow(shell);
    int target_burrow_position = get_target_burrow_location(shell);
    if (shell.location_ == Location::eHallway) {
      if (board.burrows_filled_[target_burrow] >= 0) {
        bool blocked = false;
        if (shell.position_ < target_burrow_position) {
          for (int i = shell.position_ + 1; !blocked && i < target_burrow_position; ++i) {
            if (board.hallway_[i] != '.') {
              blocked = true;
            }
          }
        }
        else {
          for (int i = shell.position_ - 1; !blocked && i > target_burrow_position; --i) {
            if (board.hallway_[i] != '.') {
              blocked = true;
            }
          }
        }
        if (!blocked) {
          int hallway_distance = abs(target_burrow_position - shell.position_);
          int burrow_distance  = board.burrow_depth_ - board.burrows_filled_[target_burrow];
          retval.push_back( { static_cast<Location>(target_burrow), burrow_distance, hallway_distance + burrow_distance });
        }
      }
    }
    else {
      bool clear = true;
      for (int test = shell.position_; clear && test > 0; --test) {
        if (board.burrows_[static_cast<int>(shell.location_)][test - 1] != '.') {
          clear = false;
        }
      }
      if (clear) {
        int current_hallway_position = 2 * static_cast<int>(shell.location_) + 2;
        for (int i = current_hallway_position; i >= 0; --i) {
          if (board.hallway_[i] != '.') {
            break;
          }
          if (i == 0 || (i % 2) == 1) {
            int hallway_distance = abs(current_hallway_position - i);
            int burrow_distance  = shell.position_ + 1;
            retval.push_back( { Location::eHallway, i, hallway_distance + burrow_distance });
          }
        }
        for (int i = current_hallway_position; i < board.hallway_.size(); ++i) {
          if (board.hallway_[i] != '.') {
            break;
          }
          if (i == 10 || (i % 2) == 1) {
            int hallway_distance = abs(current_hallway_position - i);
            int burrow_distance  = shell.position_ + 1;
            retval.push_back( { Location::eHallway, i, hallway_distance + burrow_distance });
          }
        }
      }
    }
    
    return retval;
  }
  
  std::unordered_map<std::string, int> memory;
  int best_score = INT_MAX;
  std::deque<Board> best_boards;
  
  void solve(const Board& board, int current_score, std::deque<Board>& boards)
  {
    for (size_t i = 0; i < board.available_shells_.size(); ++i) {
      auto shell = board.available_shells_[i];
      std::vector<Move> moves = get_moves(board, shell);
      if (moves.empty()) {
        continue;
      }
      for (auto move : moves) {
        Board next_board = board;
        int this_score = current_score + move.distance_ * shell.get_cost();
        if (move.location_ != Location::eHallway) {
          // Shell is coming off the board
          next_board.available_shells_.erase(next_board.available_shells_.begin() + i);
          int target_burrow = type_to_target_burrow(shell);
          next_board.burrows_filled_[target_burrow] += 1;
          next_board.burrows_[target_burrow][next_board.burrow_depth_ - next_board.burrows_filled_[target_burrow]] = shell.type_;
          next_board.hallway_[shell.position_] = '.';
          if (next_board.available_shells_.empty()) {
            if (this_score < best_score) {
              best_score = this_score;
              best_boards = boards;
              best_boards.push_back(next_board);
            }
            return;
          }
        }
        else {
          int current_burrow = static_cast<int>(shell.location_);
          if (current_burrow == type_to_target_burrow(shell)) {
            bool already_there = true;
            for (int i = shell.position_; i < board.burrow_depth_; ++i) {
              if (board.burrows_[current_burrow][i] != shell.type_) {
                already_there = false;
              }
            }
            if (already_there) {
              next_board.available_shells_.erase(next_board.available_shells_.begin() + i);
              if (next_board.burrows_filled_[current_burrow] == -1) {
                next_board.burrows_filled_[current_burrow] = 0;
              }
              next_board.burrows_filled_[current_burrow] += 1;
              // This shell is a no-op, so pass original score
              solve(next_board, current_score, boards);
              break;
            }
          }
          next_board.burrows_[current_burrow][shell.position_] = '.';
          next_board.hallway_[move.position_] = shell.type_;
          next_board.available_shells_[i].position_ = move.position_;
          next_board.available_shells_[i].location_ = Location::eHallway;
          if (shell.position_ == board.burrow_depth_ - 1) {
            next_board.burrows_filled_[current_burrow] = 0;
          }
        }
        std::string board_state = get_board_state(next_board);
        auto memo = memory.find(board_state);
        if (memo == memory.end() || this_score < memo->second) {
          boards.push_back(next_board);
          solve(next_board, this_score, boards);
          memory.insert( { board_state, this_score });
          boards.pop_back();
        }
      }
    }
  }
  
  Board initialize(std::array<std::string, 4> initial_burrows)
  {
    Board board(initial_burrows[0].size());
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < initial_burrows[0].size(); ++j) {
        board.burrows_[i][j] = initial_burrows[i][j];
        board.available_shells_.emplace_back( Shell({ initial_burrows[i][j], static_cast<Location>(i), j }) );
      }
    }
    return board;
  }
}

int main(int argc, const char * argv[]) {
  Board board = initialize({ { {"BA"}, {"CD"}, {"BC"}, {"DA"}}});
  std::deque<Board> moves;
  moves.push_back(board);
  solve(board, 0, moves);
  assert(best_score == 12521);
  for (auto print_board : best_boards) {
    dump(print_board);
  }
  std::cout << "Test: " << best_score  << "\n\n\n";
  best_score = INT_MAX;
  best_boards.clear();
  memory.clear();
  moves.clear();
  
  board = initialize({ { {"DC"}, {"CD"}, {"AA"}, {"BB"} } });
  moves.push_back(board);
  solve(board, 0, moves);
  assert(best_score == 17400);
  for (auto print_board : best_boards) {
    dump(print_board);
  }
  std::cout << "Part One: " << best_score  << "\n\n\n";
  best_score = INT_MAX;
  best_boards.clear();
  memory.clear();
  moves.clear();
  
  board = initialize({{ {"DDDC"}, {"CCBD"}, {"ABAA"}, {"BACB"} } });
  moves.push_back(board);
  solve(board, 0, moves);
  assert(best_score == 46120);
  for (auto print_board : best_boards) {
    dump(print_board);
  }
  std::cout << "Part Two: " << best_score << "\n";
  
  return 0;
}
