#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <queue>
#include <map>

#ifdef _MSC_VER
#include <assert.h>
#endif

namespace
{
  struct ArithmeticLogicUnit;

  struct Value
  {
    virtual long long get(ArithmeticLogicUnit * unit) = 0;
  };

  struct IntegerValue : public Value
  {
    IntegerValue(long long v) : v_(v) {}
    long long get(ArithmeticLogicUnit *) override { return v_; }
    long long v_;
  };

  struct Operation
  {
    virtual void execute(ArithmeticLogicUnit * unit) = 0;
  };

  struct ArithmeticLogicUnit
  {
    ArithmeticLogicUnit(const std::vector<Operation *>& program)
    : program_(program), w_(0), x_(0), y_(0), z_(0)
    {
      
    }
    
    //bool set(const std::string& input)
    bool set(const std::string& input)
    {
      std::queue<char>().swap(input_);
      for (char c : input) {
        if (c == '0') {
          return false;
        }
        input_.push(c - '0');
      }
      w_ = 0;
      x_ = 0;
      y_ = 0;
      z_ = 0;
      return true;
    }
    
    long long run()
    {
      for (auto op : program_) {
        op->execute(this);
      }
      return z_;
    }
    
    long long w_, x_, y_, z_;
    std::queue<char> input_;
    std::vector<Operation *> program_;
  };

struct AddOperation : public Operation
{
  AddOperation(char variable, Value * value) : variable_(variable), value_(value) {}
  void execute(ArithmeticLogicUnit * unit) override
  {
    switch (variable_) {
      case 'w':
        unit->w_ += value_->get(unit);
        break;
      case 'x':
        unit->x_ += value_->get(unit);
        break;
      case 'y':
        unit->y_ += value_->get(unit);
        break;
      case 'z':
        unit->z_ += value_->get(unit);
        break;
    }
  }
  
private:
  Value * value_;
  char variable_;
};

struct MulOperation : public Operation
{
  MulOperation(char variable, Value * value) : variable_(variable), value_(value) {}
  void execute(ArithmeticLogicUnit * unit) override
  {
    switch (variable_) {
      case 'w':
        unit->w_ *= value_->get(unit);
        break;
      case 'x':
        unit->x_ *= value_->get(unit);
        break;
      case 'y':
        unit->y_ *= value_->get(unit);
        break;
      case 'z':
        unit->z_ *= value_->get(unit);
        break;
    }
  }
  
private:
  Value * value_;
  char variable_;
};

struct DivOperation : public Operation
{
  DivOperation(char variable, Value * value) : variable_(variable), value_(value) {}
  void execute(ArithmeticLogicUnit * unit) override
  {
    switch (variable_) {
      case 'w':
        unit->w_ /= value_->get(unit);
        break;
      case 'x':
        unit->x_ /= value_->get(unit);
        break;
      case 'y':
        unit->y_ /= value_->get(unit);
        break;
      case 'z':
        unit->z_ /= value_->get(unit);
        break;
    }
  }
  
private:
  Value * value_;
  char variable_;
};

struct ModOperation : public Operation
{
  ModOperation(char variable, Value * value) : variable_(variable), value_(value) {}
  void execute(ArithmeticLogicUnit * unit) override
  {
    switch (variable_) {
      case 'w':
        unit->w_ %= value_->get(unit);
        break;
      case 'x':
        unit->x_ %= value_->get(unit);
        break;
      case 'y':
        unit->y_ %= value_->get(unit);
        break;
      case 'z':
        unit->z_ %= value_->get(unit);
        break;
    }
  }
  
private:
  Value * value_;
  char variable_;
};

struct EqlOperation : public Operation
{
  EqlOperation(char variable, Value * value) : variable_(variable), value_(value) {}
  void execute(ArithmeticLogicUnit * unit) override
  {
    switch (variable_) {
      case 'w':
        unit->w_ = static_cast<bool>(unit->w_ == value_->get(unit));
        break;
      case 'x':
        unit->x_ = static_cast<bool>(unit->x_ == value_->get(unit));
        break;
      case 'y':
        unit->y_ = static_cast<bool>(unit->y_ == value_->get(unit));
        break;
      case 'z':
        unit->z_ = static_cast<bool>(unit->z_ == value_->get(unit));
        break;
    }
  }
  
private:
  Value * value_;
  char variable_;
};

struct InpOperation : public Operation
{
  InpOperation(char variable) : variable_(variable) {}
  void execute(ArithmeticLogicUnit * unit) override
  {
    assert(!unit->input_.empty());
    int input = unit->input_.front();
    unit->input_.pop();
    switch (variable_) {
      case 'w':
        unit->w_ = input;
        break;
      case 'x':
        unit->x_ = input;
        break;
      case 'y':
        unit->y_ = input;
        break;
      case 'z':
        unit->y_ = input;
        break;
    }
  }
  
private:
  Value * value_;
  char variable_;
};

  struct VariableValue : public Value
  {
    VariableValue(char c) : c_(c) {}
    long long get(ArithmeticLogicUnit * unit) override
    {
      switch (c_) {
        case 'w':
          return unit->w_;
        case 'x':
          return unit->x_;
        case 'y':
          return unit->y_;
        case 'z':
          return unit->z_;
      }
      assert(false);
      return -1;
    }
    char c_;
  };
}

int main()
{
  std::ifstream input("AoC_24_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  std::vector<Operation *> commands;
  do {
    char buf[5000];
    input.getline(buf, 5000);
    std::stringstream string_input(buf);
    std::string command;
    string_input >> command;
    if (command == "inp") {
      char variable;
      string_input >> variable;
      commands.push_back(new InpOperation(variable));
    }
    else {
      char variable;
      string_input >> variable;
      std::string string_value;
      string_input >> string_value;
      Value * value;
      if (string_value[0] >= 'w' && string_value[0] <= 'z') {
        value = new VariableValue(string_value[0]);
      }
      else {
        value = new IntegerValue(std::stoi(string_value));
      }
      Operation * op;
      if (command == "add") {
        op = new AddOperation(variable, value);
      }
      else if (command == "mul") {
        op = new MulOperation(variable, value);
      }
      else if (command == "div") {
        op = new DivOperation(variable, value);
      }
      else if (command == "mod") {
        op = new ModOperation(variable, value);
      }
      else if (command == "eql") {
          op = new EqlOperation(variable, value);
      }
      commands.push_back(op);
    }
  } while (!input.eof());
  
  ArithmeticLogicUnit unit(commands);
  unit.set("99299513899971"); // Part One
  assert(unit.run() == 0);
  
  unit.set("93185111127911"); // Part Two
  assert(unit.run() == 0);
  
  return 0;
}
