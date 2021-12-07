#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <array>

typedef std::array<int, 5> BingoLine;
typedef std::array<BingoLine, 5> BingoBoard;

namespace
{
  void process_file(std::ifstream& input, std::vector<int>& numbers, std::vector<BingoBoard>& boards)
  {
    char buffer[5000];
    input.getline(buffer, 5000);
    std::stringstream string_buffer(buffer);
    while (!string_buffer.eof()) {
      int n;
      char c;
      string_buffer >> n;
      numbers.push_back(n);
      string_buffer >> c;
    }
    
    while (!input.eof()) {
      BingoBoard nextBoard;
      for (int x = 0; x < 5; ++x) {
        BingoLine nextLine;
        for (int y = 0; y < 5; ++y) {
          int n;
          input >> n;
          nextLine[y] = n;
        }
        nextBoard[x] = nextLine;
      }
      boards.emplace_back(nextBoard);
    };
  }

  bool markBoard(BingoBoard& thisBoard, int n)
  {
    for (int x = 0; x < 5; ++x) {
      for (int y = 0; y < 5; ++y) {
        if (thisBoard[x][y] == n) {
          thisBoard[x][y] = -1;
          return true;
        }
      }
    }
    return false;
  }

  bool checkBoard(const BingoBoard& thisBoard)
  {
    bool unfinished_row;
    std::array<bool, 5> unfinished_col = { false, false, false, false, false };
    for (int x = 0; x < 5; ++x) {
      unfinished_row = false;
      for (int y = 0; y < 5; ++y) {
        if (thisBoard[x][y] >= 0) {
          unfinished_row = true;
          unfinished_col[y] = true;
        }
      }
      if (!unfinished_row) {
        return true;
      }
    }
    
    for (int i = 0; i < 5; ++i) {
      if (unfinished_col[i] == false) {
        return true;
      }
    }
    
    return false;
  }

  int scoreBoard(const BingoBoard& thisBoard)
  {
    int score = 0;
    for (int x = 0; x < 5; ++x) {
      for (int y = 0; y < 5; ++y) {
        if (thisBoard[x][y] >= 0) {
          score += thisBoard[x][y];
        }
      }
    }
    return score;
  }
}

int main()
{
  std::ifstream input("AoC_4_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file" << std::endl;
    return 1;
  }
  
  std::vector<int> numbers;
  std::vector<BingoBoard> boards;
  process_file(input, numbers, boards);
  
  int firstWinner = -1, firstWinnerNumber = -1;
  int lastWinner  = -1, lastWinnerNumber  = -1;
  std::vector<char> finished(boards.size(), false);
  for (int n : numbers) {
    for (int b = 0; b < boards.size(); ++b) {
      if (!finished[b] && markBoard(boards[b], n) && checkBoard(boards[b])) {
        finished[b] = true;
        if (firstWinnerNumber < 0) {
          firstWinnerNumber = n;
          firstWinner = b;
        }
        lastWinnerNumber = n;
        lastWinner = b;
      }
    }
  }
  
  std::cout << "Part 1: " << scoreBoard(boards[firstWinner]) * firstWinnerNumber << std::endl;
  std::cout << "Part 2: " << scoreBoard(boards[lastWinner])  * lastWinnerNumber  << std::endl;
  
  return 0;
}
