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

  void process_file(std::ifstream& input, SFPairs& pairs)
  {
    do {
      char buf[5000];
      input.getline(buf, 5000);      std::string line(buf);
      pairs.push_back(parse(line));
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

  std::string dump(SFBasePtr item)
  {
    if (item->is_number()) {
      return std::to_string(std::static_pointer_cast<SFNumber>(item)->number_);
    }
    else {
      std::shared_ptr<SFPair> pair = std::static_pointer_cast<SFPair>(item);
      return std::string("[") + dump(pair->left_) + "," + dump(pair->right_) + "]";
    }
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
  
//  auto test = parse("[[[[0,7],4],[[7,8],[0,[6,7]]]],[1,1]]");
//  bool exploded = false;
//  explode(test, 0, exploded);
//  std::cout << dump(test) << "\n";
//  return 1;
  
  SFPairs pairs;
#if 1
  process_file(input, pairs);
#else
  pairs = {
#if 1
    parse("[[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]"),
    parse("[[[5,[2,8]],4],[5,[[9,9],0]]]"),
    parse("[6,[[[6,2],[5,6]],[[7,6],[4,7]]]]"),
    parse("[[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]"),
    parse("[[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]"),
    parse("[[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]"),
    parse("[[[[5,4],[7,7]],8],[[8,3],8]]"),
    parse("[[9,3],[[9,9],[6,[4,9]]]]"),
    parse("[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]"),
    parse("[[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]])"),
#else
    parse("[[[[4,3],4],4],[7,[[8,4],9]]]"),
    parse("[1,1]")
#endif
  };
#endif
  std::shared_ptr<SFPair> work_pair = pairs[0];
  for (int i = 1; i < pairs.size(); ++i) {
    work_pair = add(work_pair, pairs[i]);
    while (true) {
      bool exploded = false;
      explode(work_pair, 0, exploded);
      if (!exploded && !split(work_pair)) {
        break;
      }
    }
  }
  
  std::cout << "Part One: " << get_magnitude(work_pair) << "\n";
  
  //std::cout << "Part One: " << bitsMachine.version_sum_ << "\n";
  //std::cout << "Part Two: " << result << std::endl;
  
  return 0;
}
