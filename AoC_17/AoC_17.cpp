#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <queue>

namespace
{
typedef std::queue<char> Bits;

  void process_file(std::ifstream& input, Bits& bits)
  {
    do {
      char c;
      input >> c;
      if (!input.eof()) {
        int actual;
        if (c >= '0' && c <= '9') {
          actual = c - '0';
        }
        else {
          actual = c - 'A' + 10;
        }
        bits.push(static_cast<bool>(actual & 8));
        bits.push(static_cast<bool>(actual & 4));
        bits.push(static_cast<bool>(actual & 2));
        bits.push(static_cast<bool>(actual & 1));
      }
    } while (!input.eof());
  }


 struct BitsMachine
 {
   BitsMachine(Bits bits) : bits_(bits), version_sum_(0) {}
   
   char read()
   {
     char c = bits_.front();
     bits_.pop();
     return c;
   }
   
   int read_number(int bits_to_read)
   {
     int retval = 0;
     while (bits_to_read > 0) {
       retval += (read() << --bits_to_read);
     }
     return retval;
   }
   
   long long read_literal_value()
   {
     long long retval = 0;
     bool is_last_byte = false;
     do {
       is_last_byte = (read() == 0);
       retval <<= 4;
       retval += read_number(4);
     }
     while (!is_last_byte);
     return retval;
   }

   long long read_packet()
   {
     int version = read_number(3);
     version_sum_ += version;
     int type_id = read_number(3);
     if (type_id == 4) {
       return read_literal_value();
     }
     else {
       std::vector<long long> subpackets;
       // Next bit is length type
       if (read() == 0) {
         size_t bit_length = read_number(15);
         size_t read_end = bits_.size() - bit_length;
         while (bits_.size() != read_end) {
           subpackets.push_back(read_packet());
         }
       }
       else {
         int packet_length = read_number(11);
         for (int i = 0; i < packet_length; ++i) {
           subpackets.push_back(read_packet());
         }
       }
       switch (type_id)
       {
         case 0:
           return std::accumulate(subpackets.begin(), subpackets.end(), 0ll);
         case 1:
           return std::accumulate(subpackets.begin(), subpackets.end(), 1ll, std::multiplies<long long>());
         case 2:
           return *std::min_element(subpackets.begin(), subpackets.end());
         case 3:
           return *std::max_element(subpackets.begin(), subpackets.end());
         case 5:
           return subpackets[0] > subpackets[1];
         case 6:
           return subpackets[0] < subpackets[1];
         case 7:
           return subpackets[0] == subpackets[1];
       }
     }
     
     assert(false);
     
     return -1;
   }
   
   Bits bits_;
   int version_sum_;
 };
}

int main()
{
  std::ifstream input("AoC_17_input.dat");
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  
  Bits bits;
  process_file(input, bits);
  BitsMachine bitsMachine(bits);
  
  long long result = bitsMachine.read_packet();
  
  std::cout << "Part One: " << bitsMachine.version_sum_ << "\n";
  std::cout << "Part Two: " << result << std::endl;
  
  return 0;
}
