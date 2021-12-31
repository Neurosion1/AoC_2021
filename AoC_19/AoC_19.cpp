#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <unordered_set>

namespace
{
  struct Point3D
  {
    int x_, y_, z_;
    
    bool operator<(const Point3D& other) const
    {
      return x_ < other.x_ || y_ < other.y_ || z_ < other.z_;
    }
  };

  struct Beacon
  {
    std::map<int, float> distance_to_other_beacons_;
    Point3D location_;
  };

  struct Scanner
  {
    std::vector<Beacon> beacons_;
  };

  float get_distance(Point3D p1, Point3D p2)
  {
    return sqrt((p2.x_ - p1.x_) * (p2.x_ - p1.x_) +
                (p2.y_ - p1.y_) * (p2.y_ - p1.y_) +
                (p2.z_ - p1.z_) * (p2.z_ - p1.z_));
  }

  void process_file(std::ifstream& input, std::vector<Scanner>& scanners)
  {
    do {
      char buffer[5000];
      input.getline(buffer, 5000);
      std::vector<Point3D> raw_beacons;
      do {
        Point3D this_location;
        input.getline(buffer, 5000);
        if (strlen(buffer) == 0) {
          break;
        }
        std::stringstream str(buffer);
        char comma;
        str >> this_location.x_ >> comma >> this_location.y_ >> comma >> this_location.z_;
        raw_beacons.push_back(this_location);
      } while (true);
      Scanner this_scanner;
      this_scanner.beacons_.resize(raw_beacons.size());
      for (int b1 = 0; b1 < raw_beacons.size() - 1; ++b1) {
        for (int b2 = b1 + 1; b2 < raw_beacons.size(); ++b2) {
          float distance = get_distance(raw_beacons[b1], raw_beacons[b2]);
          auto iter = this_scanner.beacons_[b1].distance_to_other_beacons_.insert( { b2, distance} );
          assert(iter.second != false);
          this_scanner.beacons_[b2].distance_to_other_beacons_.insert( { b1, distance} );
          assert(iter.second != false);
        }
      }
      scanners.push_back(this_scanner);
    } while (!input.eof());
  }

  void find_third_point(const Scanner& this_scanner, const Scanner& that_scanner,
                        const Beacon& this_beacon_1, const Beacon& this_beacon_2,
                        const Beacon& that_beacon_1, const Beacon& that_beacon_2)
  {
    for (int b1_index = 0; b1_index < this_beacon_1.distance_to_other_beacons_.size(); ++b1_index) {
      for (int b2_index = 0; b2_index < this_beacon_2.distance_to_other_beacons_.size(); ++b2_index) {
        
      }
    }
  }

  int match_beacons(const std::vector<Scanner>& scanners, int this_scanner_index)
  {
    int candidates = 0;
    const Scanner & this_scanner = scanners[this_scanner_index];
    for (int this_beacon_index = 0; this_beacon_index < this_scanner.beacons_.size(); ++this_beacon_index) {
      const Beacon & this_scanner_this_beacon = this_scanner.beacons_[this_beacon_index];
      for (auto this_scanner_that_beacon : this_scanner_this_beacon.distance_to_other_beacons_) {
        float this_distance = this_scanner_that_beacon.second;
        for (int that_scanner_index = 0; that_scanner_index < scanners.size(); ++that_scanner_index) {
          if (that_scanner_index != this_scanner_index) {
            const Scanner & that_scanner = scanners[that_scanner_index];
            for (int that_beacon_index = 0; that_beacon_index < that_scanner.beacons_.size(); ++that_beacon_index) {
              const Beacon & that_scanner_this_beacon = that_scanner.beacons_[that_beacon_index];
              for (auto that_scanner_that_beacon : that_scanner_this_beacon.distance_to_other_beacons_) {
                float that_distance = that_scanner_that_beacon.second;
                if (this_distance == that_distance) {
                  find_third_point(this_scanner, that_scanner,
                                   this_scanner_this_beacon,
                                   this_scanner.beacons_[this_scanner_that_beacon.first],
                                   that_scanner_this_beacon,
                                   that_scanner.beacons_[that_scanner_that_beacon.first]);
                }
              }
            }
          }
        }
      }
    }
    return candidates;
  }
}

int main()
{
  std::ifstream input("AoC_19_input.dat");
  
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  std::vector<Scanner> scanners;
  process_file(input, scanners);
  
  int candidates = match_beacons(scanners, 0);
  
//  std::multimap<float, int> distances;
//  for (int s = 0; s < scanners.size(); ++s) {
//    auto & beacons = scanners[s].beacons_;
//    for (int b1 = 0; b1 < beacons.size() - 1; ++b1) {
//      for (int b2 = b1 + 1; b2 < beacons.size(); ++b2) {
//        distances.insert( { get_distance(beacons[b1], beacons[b2]), s } );
//      }
//    }
//  }
  
//  std::vector<float> detected;
//  for (auto iter = distances.begin(); iter != distances.end();) {
//    if (distances.count(iter->first) >= 3 && iter->second == 0) {
//      detected.push_back(iter->first);
//    }
//    iter = distances.upper_bound(iter->first);
//  }
  
  //std::cout << "Part One: " << bitsMachine.version_sum_ << "\n";
  //std::cout << "Part Two: " << result << std::endl;
  // 84?
  return 0;
}

