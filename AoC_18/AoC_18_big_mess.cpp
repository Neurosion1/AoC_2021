#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <queue>
#include <memory>

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
    Side side_;
    Type type_;
    std::weak_ptr<SFPair> parent_;
    virtual bool is_number() const = 0;
  };

  struct SFNumber : public SFBase
  {
    SFNumber(int number) : number_(number) {}
    long long number_;
    virtual bool is_number() const override { return true; }
  };

  struct SFPair : public SFBase
  {
    SFBasePtr left_;
    SFBasePtr right_;
    virtual bool is_number() const override { return false; }
  };

  std::shared_ptr<SFPair> parse_impl(const std::string& text, int& position, std::shared_ptr<SFPair> parent)
  {
    std::shared_ptr<SFPair> retval = std::make_shared<SFPair>();
    retval->parent_ = parent;
    
    assert(text[position] == '[');
    if (text[++position] == '[') {
      retval->left_ = parse_impl(text, position, retval);
      retval->left_->side_ = Side::eLeft;
    }
    else {
      assert(text[position] >= '0' && text[position] <= '9');
      retval->left_ = std::make_shared<SFNumber>(text[position++] - '0');
    }
    assert(text[position] == ',');
    if (text[++position] == '[') {
      retval->right_ = parse_impl(text, position, retval);
      retval->right_->side_ = Side::eRight;
    }
    else {
      assert(text[position] >= '0' && text[position] <= '9');
      retval->right_ = std::make_shared<SFNumber>(text[position++] - '0');
    }
    assert(text[position] == ']');
    ++position;
           
    return retval;
  }

  std::shared_ptr<SFPair> parse(const std::string& text)
  {
    int position = 0;
    return parse_impl(text, position, nullptr);
  }

  void process_file(std::ifstream& input, SFPairs& pairs)
  {
    do {
      char buf[5000];
      input.getline(buf, 5000);
      int position = 0;
      std::string line(buf);
      pairs.push_back(parse(line));
    } while (!input.eof());
  }

  std::shared_ptr<SFPair> add(std::shared_ptr<SFPair> left, std::shared_ptr<SFPair> right)
  {
    std::shared_ptr<SFPair> retval = std::make_shared<SFPair>();
    retval->left_ = left;
    retval->left_->parent_ = retval;
    retval->right_ = right;
    retval->right_->parent_ = retval;
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
    if (depth == 4) {
      auto parent = pair->parent_.lock();
      while (parent && parent->side_ == Side::eLeft) {
        parent = parent->parent_.lock();
      }
      if (parent) {
        auto child = parent->left_;
        while (!child->is_number()) {
          child = std::static_pointer_cast<SFPair>(child)->right_;
        }
        std::static_pointer_cast<SFNumber>(child)->number_ += std::static_pointer_cast<SFNumber>(pair->left_)->number_;
      }
      
      parent = pair->parent_.lock();
      while (parent && parent->side_ == Side::eRight) {
        parent = parent->parent_.lock();
      }
      if (parent) {
        auto child = parent->right_;
        while (!child->is_number()) {
          child = std::static_pointer_cast<SFPair>(child)->left_;
        }
        std::static_pointer_cast<SFNumber>(child)->number_ += std::static_pointer_cast<SFNumber>(pair->right_)->number_;
      }
      exploded = true;
    }
    else {
      if (!pair->left_->is_number()) {
        if (explode(std::static_pointer_cast<SFPair>(pair->left_), depth + 1, exploded)) {
          pair->left_ = std::make_shared<SFNumber>(0);
        }
      }
      if (!exploded && !pair->right_->is_number()) {
        if (explode(std::static_pointer_cast<SFPair>(pair->right_), depth + 1, exploded)) {
          pair->right_ = std::make_shared<SFNumber>(0);
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
        std::shared_ptr<SFPair> new_pair = std::make_shared<SFPair>();
        new_pair->left_  = std::make_shared<SFNumber>(number->number_ / 2);
        new_pair->right_ = std::make_shared<SFNumber>(number->number_ / 2 + (number->number_ % 2 != 0));
        new_pair->parent_ = pair;
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
        std::shared_ptr<SFPair> new_pair = std::make_shared<SFPair>();
        new_pair->left_  = std::make_shared<SFNumber>(number->number_ / 2);
        new_pair->right_ = std::make_shared<SFNumber>(number->number_ / 2 + (number->number_ % 2 != 0));
        new_pair->parent_ = pair;
        pair->right_ = new_pair;
        return true;
      }
    }
    else if (split(std::static_pointer_cast<SFPair>(pair->right_))) {
      return true;
    }
    
    return false;
  }

  //bool split(std::shared_ptr<SFPair> pair)
  //{
    
  //}
}

int main()
{
  std::ifstream input("AoC_18_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  SFPairs pairs;
#if 0
  process_file(input, pairs);
#else
  pairs = {
    parse("[1,1]"),
    parse("[2,2]"),
    parse("[3,3]"),
    parse("[4,4]"),
    parse("[5,5]"),
  };
#endif
  std::shared_ptr<SFPair> work_pair = pairs[0];
  for (int i = 1; i < pairs.size(); ++i) {
    work_pair = add(work_pair, pairs[i]);
    std::cout << std::setw(16) << "After addition: " << dump(work_pair) << "\n";
    while (true) {
      bool exploded = false;
      explode(work_pair, 0, exploded);
      if (exploded) {
        std::cout << std::setw(16) << "After explode: " << dump(work_pair) << "\n";
      }
      else {
        if (split(work_pair)) {
          std::cout << std::setw(16) << "After split: " << dump(work_pair) << "\n";
        }
        else {
          break;
        }
      }
    }
  }
  
  
  //std::cout << "Part One: " << bitsMachine.version_sum_ << "\n";
  //std::cout << "Part Two: " << result << std::endl;
  
  return 0;
}
