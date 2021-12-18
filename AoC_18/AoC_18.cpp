#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <queue>
#include <memory>
#include <algorithm>

namespace
{
  struct SFBase;
  typedef std::shared_ptr<SFBase> SFBasePtr;
  struct SFPair;
  typedef std::vector<std::shared_ptr<SFPair>> SFPairs;
  struct SFNumber;

  enum class Side { eLeft, eRight };
  enum class Type { eNumber, ePair };
  struct SFBase
  {
    SFBase(std::shared_ptr<SFPair> parent, Side side)
      : side_(side), parent_(parent)
    {
      
    }
    
    Side side_;
    std::weak_ptr<SFPair> parent_;
    virtual bool is_number() const = 0;
  };

  struct SFNumber : public SFBase
  {
    SFNumber(std::shared_ptr<SFPair> parent, Side side, int number) : SFBase(parent, side), number_(number) {}
    long long number_;
    virtual bool is_number() const override { return true; }
  };

  struct SFPair : public SFBase
  {
    SFPair(std::shared_ptr<SFPair> parent, Side side) : SFBase(parent, side) {}
    SFBasePtr left_;
    SFBasePtr right_;
    virtual bool is_number() const override { return false; }
  };

  std::shared_ptr<SFPair> parse_impl(const std::string& text, int& position, std::shared_ptr<SFPair> parent, Side side)
  {
    std::shared_ptr<SFPair> retval = std::make_shared<SFPair>(parent, side);
    retval->parent_ = parent;
    
    assert(text[position] == '[');
    if (text[++position] == '[') {
      retval->left_ = parse_impl(text, position, retval, Side::eLeft);
    }
    else {
      assert(text[position] >= '0' && text[position] <= '9');
      retval->left_ = std::make_shared<SFNumber>(retval, Side::eLeft, text[position++] - '0');
    }
    assert(text[position] == ',');
    if (text[++position] == '[') {
      retval->right_ = parse_impl(text, position, retval, Side::eRight);
    }
    else {
      assert(text[position] >= '0' && text[position] <= '9');
      retval->right_ = std::make_shared<SFNumber>(retval, Side::eRight, text[position++] - '0');
    }
    assert(text[position] == ']');
    ++position;
           
    return retval;
  }

  std::shared_ptr<SFPair> parse(const std::string& text)
  {
    int position = 0;
    return parse_impl(text, position, nullptr, Side::eLeft);
  }

  void process_file(std::ifstream& input, std::vector<std::string>& pairs)
  {
    do {
      char buf[5000];
      input.getline(buf, 5000);
      std::string line(buf);
      pairs.push_back(line);
    } while (!input.eof());
  }

  std::shared_ptr<SFPair> add(std::shared_ptr<SFPair> left, std::shared_ptr<SFPair> right)
  {
    std::shared_ptr<SFPair> retval = std::make_shared<SFPair>(nullptr, Side::eLeft);
    retval->left_ = left;
    retval->left_->parent_ = retval;
    retval->left_->side_ = Side::eLeft;
    retval->right_ = right;
    retval->right_->parent_ = retval;
    retval->right_->side_ = Side::eRight;
    return retval;
  }

  bool explode(std::shared_ptr<SFPair> pair, int depth, bool& exploded)
  {
    assert(!pair->is_number());
    if (depth == 4) {
      auto current = pair;
      while (current && current->side_ == Side::eLeft) {
        current = current->parent_.lock();
      }
      if (current) {
        auto child = current->parent_.lock()->left_;
        while (!child->is_number()) {
          child = std::static_pointer_cast<SFPair>(child)->right_;
        }
        std::static_pointer_cast<SFNumber>(child)->number_ += std::static_pointer_cast<SFNumber>(pair->left_)->number_;
      }
      
      current = pair;
      while (current && current->side_ == Side::eRight) {
        current = current->parent_.lock();
      }
      if (current && current->parent_.lock()) {
        auto child = current->parent_.lock()->right_;
        while (!child->is_number()) {
          child = std::static_pointer_cast<SFPair>(child)->left_;
        }
        std::static_pointer_cast<SFNumber>(child)->number_ += std::static_pointer_cast<SFNumber>(pair->right_)->number_;
      }
      exploded = true;
      return true;
    }
    else {
      if (!pair->left_->is_number()) {
        if (explode(std::static_pointer_cast<SFPair>(pair->left_), depth + 1, exploded)) {
          pair->left_ = std::make_shared<SFNumber>(pair, Side::eLeft, 0);
        }
      }
      if (!exploded && !pair->right_->is_number()) {
        if (explode(std::static_pointer_cast<SFPair>(pair->right_), depth + 1, exploded)) {
          pair->right_ = std::make_shared<SFNumber>(pair, Side::eRight, 0);
        }
      }
    }
    return false;
  }

  bool split(std::shared_ptr<SFPair> pair)
  {
    if (pair->left_->is_number()) {
      std::shared_ptr<SFNumber> number = std::static_pointer_cast<SFNumber>(pair->left_);
      if (number->number_ >= 10) {
        std::shared_ptr<SFPair> new_pair = std::make_shared<SFPair>(pair, Side::eLeft);
        new_pair->left_  = std::make_shared<SFNumber>(new_pair, Side::eLeft, number->number_ / 2);
        new_pair->right_ = std::make_shared<SFNumber>(new_pair, Side::eRight, number->number_ / 2 + (number->number_ % 2 != 0));
        pair->left_ = new_pair;
        return true;
      }
    }
    else if (split(std::static_pointer_cast<SFPair>(pair->left_))) {
      return true;
    }
    
    if (pair->right_->is_number()) {
      std::shared_ptr<SFNumber> number = std::static_pointer_cast<SFNumber>(pair->right_);
      if (number->number_ >= 10) {
        std::shared_ptr<SFPair> new_pair = std::make_shared<SFPair>(pair, Side::eRight);
        new_pair->left_  = std::make_shared<SFNumber>(new_pair, Side::eLeft, number->number_ / 2);
        new_pair->right_ = std::make_shared<SFNumber>(new_pair, Side::eRight, number->number_ / 2 + (number->number_ % 2 != 0));
        pair->right_ = new_pair;
        return true;
      }
    }
    else if (split(std::static_pointer_cast<SFPair>(pair->right_))) {
      return true;
    }
    
    return false;
  }

  long long get_magnitude(std::shared_ptr<SFPair> pair)
  {
    long long left, right;
    if (pair->left_->is_number()) {
      left = std::static_pointer_cast<SFNumber>(pair->left_)->number_;
    }
    else {
      left = get_magnitude(std::static_pointer_cast<SFPair>(pair->left_));
    }
    if (pair->right_->is_number()) {
      right = std::static_pointer_cast<SFNumber>(pair->right_)->number_;
    }
    else {
      right = get_magnitude(std::static_pointer_cast<SFPair>(pair->right_));
    }
    return (3 * left) + (2 * right);
  }
}

int main()
{
  std::ifstream input("AoC_18_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  std::vector<std::string> pairs;
  process_file(input, pairs);
  std::shared_ptr<SFPair> work_pair = parse(pairs[0]);
  for (int i = 1; i < pairs.size(); ++i) {
    work_pair = add(work_pair, parse(pairs[i]));
    while (true) {
      bool exploded = false;
      explode(work_pair, 0, exploded);
      if (!exploded && !split(work_pair)) {
        break;
      }
    }
  }
  
  std::cout << "Part One: " << get_magnitude(work_pair) << "\n";
  
  long long best = 0;
  for (int i = 0; i < pairs.size() - 1; ++i) {
    for (int j = 1; j < pairs.size(); ++j) {
      std::shared_ptr<SFPair> work_pair = add(parse(pairs[i]), parse(pairs[j]));
      while (true) {
        bool exploded = false;
        explode(work_pair, 0, exploded);
        if (!exploded && !split(work_pair)) {
          break;
        }
      }
      best = std::max(best, get_magnitude(work_pair));
      
      work_pair = add(parse(pairs[j]), parse(pairs[i]));
      while (true) {
        bool exploded = false;
        explode(work_pair, 0, exploded);
        if (!exploded && !split(work_pair)) {
          break;
        }
      }
      best = std::max(best, get_magnitude(work_pair));
    }
  }
  std::cout << "Part Two: " << best << "\n";
  
  //std::cout << "Part One: " << bitsMachine.version_sum_ << "\n";
  //std::cout << "Part Two: " << result << std::endl;
  
  return 0;
}
