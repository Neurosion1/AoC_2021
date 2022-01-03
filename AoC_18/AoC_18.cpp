#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <queue>
#include <memory>
#include <algorithm>

#ifdef _MSC_VER
#include <assert.h>
#endif

namespace
{
  struct SFBase;
  typedef std::shared_ptr<SFBase> SFBasePtr;
  struct SFPair;
  typedef std::vector<std::shared_ptr<SFPair>> SFPairs;
  struct SFNumber;

  enum class Side { eLeft, eRight };
  struct SFBase
  {
    SFBase(std::shared_ptr<SFPair> parent, Side side)
      : side_(side), parent_(parent)
    {
      
    }
    
    Side side_;
    std::weak_ptr<SFPair> parent_;
    virtual bool is_number() const = 0;
    virtual long long get_magnitude() const = 0;
  };

  struct SFNumber : public SFBase, public std::enable_shared_from_this<SFNumber>
  {
    SFNumber(std::shared_ptr<SFPair> parent, Side side, int number) : SFBase(parent, side), number_(number) {}
    long long number_;
    virtual bool is_number() const override { return true; }
    long long get_magnitude() const override { return number_; }
  };

  struct SFPair : public SFBase, public std::enable_shared_from_this<SFPair>
  {
    SFPair(std::shared_ptr<SFPair> parent, Side side) : SFBase(parent, side) {}
    SFBasePtr left_;
    SFBasePtr right_;
    virtual bool is_number() const override { return false; }
    long long get_magnitude() const override { return left_->get_magnitude() * 3 + right_->get_magnitude() * 2; }
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

  void do_explode(std::shared_ptr<SFPair> pair, Side side)
  {
    auto current = pair;
    while (current && current->side_ == side) {
      current = current->parent_.lock();
    }
    if (current) {
      if (!current->parent_.lock()) {
        return;
      }
      auto child = (side == Side::eLeft ? current->parent_.lock()->left_ : current->parent_.lock()->right_);
      while (auto pair = std::dynamic_pointer_cast<SFPair>(child)) {
        child = (side == Side::eLeft ? pair->right_ : pair->left_);
      }
      std::static_pointer_cast<SFNumber>(child)->number_ +=
        std::static_pointer_cast<SFNumber>(side == Side::eLeft ? pair->left_ : pair->right_)->number_;
    }
  }

  bool explode(std::shared_ptr<SFPair> pair, int depth)
  {
    assert(!pair->is_number());
    if (depth == 4) {
      do_explode(pair, Side::eLeft);
      do_explode(pair, Side::eRight);
      return true;
    }
    else {
      if (!pair->left_->is_number()) {
        if (explode(std::static_pointer_cast<SFPair>(pair->left_), depth + 1)) {
          pair->left_ = std::make_shared<SFNumber>(pair, Side::eLeft, 0);
        }
      }
      if (!pair->right_->is_number()) {
        if (explode(std::static_pointer_cast<SFPair>(pair->right_), depth + 1)) {
          pair->right_ = std::make_shared<SFNumber>(pair, Side::eRight, 0);
        }
      }
    }
    return false;
  }

  std::shared_ptr<SFPair> do_split(std::shared_ptr<SFPair> parent, Side side, long long number)
  {
    std::shared_ptr<SFPair> new_pair = std::make_shared<SFPair>(parent, side);
    new_pair->left_  = std::make_shared<SFNumber>(new_pair, Side::eLeft, number / 2);
    new_pair->right_ = std::make_shared<SFNumber>(new_pair, Side::eRight, number / 2 + (number % 2));
    return new_pair;
  }

  bool split(std::shared_ptr<SFPair> pair)
  {
    if (pair->left_->is_number()) {
      long long number = std::static_pointer_cast<SFNumber>(pair->left_)->number_;
      if (number >= 10) {
        pair->left_ = do_split(pair, Side::eLeft, number);
        return true;
      }
    }
    else if (split(std::static_pointer_cast<SFPair>(pair->left_))) {
      return true;
    }
    
    if (pair->right_->is_number()) {
      long long number = std::static_pointer_cast<SFNumber>(pair->right_)->number_;
      if (number >= 10) {
        pair->right_ = do_split(pair, Side::eRight, number);
        return true;
      }
    }
    else if (split(std::static_pointer_cast<SFPair>(pair->right_))) {
      return true;
    }
    
    return false;
  }

  void reduce(std::shared_ptr<SFPair> pair)
  {
    while (explode(pair, 0) || split(pair));
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
    reduce(work_pair);
  }
  
  std::cout << "Part One: " << work_pair->get_magnitude() << "\n";
  
  long long best = 0;
  for (int i = 0; i < pairs.size() - 1; ++i) {
    for (int j = i + 1; j < pairs.size(); ++j) {
      std::shared_ptr<SFPair> work_pair = add(parse(pairs[i]), parse(pairs[j]));
      reduce(work_pair);
      best = std::max(best, work_pair->get_magnitude());
      
      work_pair = add(parse(pairs[j]), parse(pairs[i]));
      reduce(work_pair);
      best = std::max(best, work_pair->get_magnitude());
    }
  }
  std::cout << "Part Two: " << best << "\n";
  
  return 0;
}
