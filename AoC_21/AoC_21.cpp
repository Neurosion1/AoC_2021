#include <iostream>
#include <array>

namespace
{
  class DeterministicDie
  {
  public:
    DeterministicDie() : next_roll_(1), total_rolls_(0) {}
    
    long long roll()
    {
      int total = 0;
      for (int i = 0; i < 3; ++i) {
        total += (next_roll_++);
        next_roll_ = (next_roll_ == 101 ? 1 : next_roll_);
      }
      total_rolls_ += 3;
      return total;
    }
    
    int total_rolls_;
  private:
    long long next_roll_;
  };

  const int P1_START = 8, P2_START = 1;
}

int main(int argc, const char * argv[]) {

  long long p1_score = 0, p2_score = 0;
  int p1_space = P1_START, p2_space = P2_START;
  DeterministicDie die;
  while (p1_score < 1000 && p2_score < 1000) {
    p1_space = (p1_space + die.roll()) % 10;
    p1_score += (p1_space == 0 ? 10 : p1_space);
    if (p1_score < 1000) {
      p2_space = (p2_space + die.roll()) % 10;
      p2_score += (p2_space == 0 ? 10 : p2_space);
    }
  }
  long long part_one_answer = (p1_score > p2_score ? p2_score : p1_score) * die.total_rolls_;
  std::cout << "Part one: " << part_one_answer << "\n";
  
  // Count [21 potential P1 scores][21 potential P2 scores][10 P1 spaces][10 P2 spaces]
  typedef std::array<std::array<std::array<std::array<long long, 21>, 21>, 10>, 10> GameState;
  GameState current_game_states = { 0 };
  current_game_states[P1_START][P2_START][0][0] = 1; // One start-of-game state
  long long total_game_states = 1;
  bool doing_p1 = true;
  long long wins[2] = { 0 };
  do {
    GameState next_game_states = { 0 };
    for (int p1_space = 0; p1_space < 10; ++p1_space) {
      for (int p2_space = 0; p2_space < 10; ++p2_space) {
        for (int p1_score = 0; p1_score < 21; ++p1_score) {
          for (int p2_score = 0; p2_score < 21; ++p2_score) {
            long long these_game_states = current_game_states[p1_space][p2_space][p1_score][p2_score];
            if (these_game_states > 0) {
              // Values and frequencies for the 27 theoretical ways one could roll a 3-sided die 3 times
              static const long long scores[7][2] = { {3, 1}, {4, 3}, {5, 6}, {6, 7}, {7, 6}, {8, 3}, {9, 1} };
              for (long long score = 0; score < 7; ++score) {
                long long new_game_states = these_game_states * scores[score][1];
                if (doing_p1) {
                  int new_space = (p1_space + scores[score][0]) % 10;
                  int new_score = (new_space == 0 ? 10 : new_space) + p1_score;
                  if (new_score >= 21) {
                    wins[0] += new_game_states;
                  }
                  else {
                    next_game_states[new_space][p2_space][new_score][p2_score] += new_game_states;
                    total_game_states += new_game_states;
                  }
                }
                else {
                  int new_space = (p2_space + scores[score][0]) % 10;
                  int new_score = (new_space == 0 ? 10 : new_space) + p2_score;
                  if (new_score >= 21) {
                    wins[1] += new_game_states;
                  }
                  else {
                    next_game_states[p1_space][new_space][p1_score][new_score] += new_game_states;
                    total_game_states += new_game_states;
                  }
                }
              }
              total_game_states -= these_game_states;
            }
          }
        }
      }
    }
    current_game_states = next_game_states;
    doing_p1 = !doing_p1;
  }
  while (total_game_states > 0);
  std::cout << (wins[0] > wins[1] ? wins[0] : wins[1]) << "\n";
  
  return 0;
}
