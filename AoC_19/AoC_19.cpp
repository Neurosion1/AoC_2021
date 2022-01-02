#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <algorithm>
#include <sstream>
#include <set>
#include <list>
#include <unordered_map>

namespace
{
  struct Point3D
  {
    Point3D() : x_(0), y_(0), z_(0) {}
    Point3D(int x, int y, int z) : x_(x), y_(y), z_(z) {}
    
    bool operator<(const Point3D& other) const
    {
      if (x_ != other.x_) {
        return x_ < other.x_;
      }
      if (y_ != other.y_) {
        return y_ < other.y_;
      }
      return z_ < other.z_;
    }
    
    int x_, y_, z_;
  };
  
  // Use the Manhattan distance for the convenience of solving part two.
  int get_distance(Point3D p1, Point3D p2)
  {
    return abs(p2.x_ - p1.x_) + abs(p2.y_ - p1.y_) + abs(p2.z_ - p1.z_);
  }
  
  struct Beacon
  {
    std::unordered_map<std::string, int> neighbors_;
    Point3D location_;
  };

  // The "fingerprint" for a beacon is determined by identifying the beacon's
  // neighbors by distance. This distance should be the same for matchine pairs
  // of beacons regardless of the coordinates and axes of the scanners observing
  // them. Unfortunately for my "real" data, distance alone was not sufficient to
  // uniquely identify a neighbor. The max offset gives me the uniqueness I need,
  // and although I'm suspicious that this is insufficient for all theoretical
  // data, it works for MY data, so we'll live with it.
  std::string get_fingerprint(const Point3D& this_beacon, const Point3D& that_beacon)
  {
    int x_offset = abs(this_beacon.x_ - that_beacon.x_);
    int y_offset = abs(this_beacon.y_ - that_beacon.y_);
    int z_offset = abs(this_beacon.z_ - that_beacon.z_);
    
    std::stringstream str;
    str << std::to_string(get_distance(this_beacon, that_beacon))
        << '/' << std::max(x_offset, std::max(y_offset, z_offset));
    
    return str.str();
  }

  struct Scanner
  {
    std::vector<Beacon> beacons_;
    Point3D position_;
  };

  void process_file(std::ifstream& input, std::list<Scanner>& scanners)
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
      for (int i = 0; i < this_scanner.beacons_.size(); ++i) {
        this_scanner.beacons_[i].location_ = raw_beacons[i];
      }
      for (int b1 = 0; b1 < raw_beacons.size() - 1; ++b1) {
        for (int b2 = b1 + 1; b2 < raw_beacons.size(); ++b2) {
          std::string fingerprint = get_fingerprint(raw_beacons[b1], raw_beacons[b2]);
          auto iter = this_scanner.beacons_[b1].neighbors_.insert( { fingerprint, b2 });
          assert(iter.second != false);
          iter = this_scanner.beacons_[b2].neighbors_.insert( { fingerprint, b1 });
          assert(iter.second != false);
        }
      }
      scanners.push_back(this_scanner);
    } while (!input.eof());
  }
  
  // Given an aligned scanner (i.e. a scanner with a known position in relation
  // to the "home" scanner with its beacons adjusted appropriately), find a
  // beacon in that scanner that matches a beacon in the unaligned scanner. We
  // determine this "match" by confirming that the unaligned beacon has the same
  // neighbors (determined by distance) as the aligned beacon. Then adjust the
  // unaligned beacons to match the axes and coordinate system as the aligned
  // beacons.
  bool align_scanners(const Scanner& aligned_scanner, Scanner& unaligned_scanner)
  {
    for (int i = 0; i < aligned_scanner.beacons_.size(); ++i) {
      for (int j = 0; j < unaligned_scanner.beacons_.size(); ++j) {
        std::vector<std::pair<const Beacon&, Beacon&>> matching_beacons;
        const Beacon & aligned_beacon = aligned_scanner.beacons_[i];
        Beacon & unaligned_beacon = unaligned_scanner.beacons_[j];
        const auto & aligned_neighbors = aligned_beacon.neighbors_;
        const auto & unaligned_neighbors = unaligned_beacon.neighbors_;
        for (auto aligned_neighbor : aligned_neighbors) {
          auto unaligned_neighbor_iter = unaligned_neighbors.find(aligned_neighbor.first);
          if (unaligned_neighbor_iter != unaligned_neighbors.end()) {
            matching_beacons.push_back({
              aligned_scanner.beacons_[aligned_neighbor.second], unaligned_scanner.beacons_[unaligned_neighbor_iter->second]});
          }
        }
        
        // We are told that we can assume a match if the two scanners share
        // 12 points in common, which would be the beacon we're looking at
        // and 11 common connections.
        if (matching_beacons.size() < 11) {
          continue;
        }
        
        for (auto match : matching_beacons) {
          auto aligned_location = match.first.location_;
          int aligned_beacon_dx = aligned_location.x_ - aligned_beacon.location_.x_;
          int aligned_beacon_dy = aligned_location.y_ - aligned_beacon.location_.y_;
          int aligned_beacon_dz = aligned_location.z_ - aligned_beacon.location_.z_;
          
          auto unaligned_location = match.second.location_;
          int unaligned_beacon_dx = unaligned_location.x_ - unaligned_beacon.location_.x_;
          int unaligned_beacon_dy = unaligned_location.y_ - unaligned_beacon.location_.y_;
          int unaligned_beacon_dz = unaligned_location.z_ - unaligned_beacon.location_.z_;
          
          // We need to find a pair of matching beacons with unique non-zero deltas
          // on every axis. Once we have that pair, we'll be able to discover the
          // exact transformation of the unaligned beacon and fix it.
          if (abs(aligned_beacon_dx) == abs(aligned_beacon_dy) ||
              abs(aligned_beacon_dx) == abs(aligned_beacon_dz) ||
              abs(aligned_beacon_dy) == abs(aligned_beacon_dz) ||
              aligned_beacon_dx * aligned_beacon_dy * aligned_beacon_dz == 0)
                  continue;
          
          // Finds the unaligned axis whose delta matches the given aligned
          // axis delta and returns that axis.
          auto alignment_func = [unaligned_beacon_dx, unaligned_beacon_dy, unaligned_beacon_dz](bool& negate, int aligned_delta)
          {
            if (abs(unaligned_beacon_dx) == abs(aligned_delta)) {
              negate = (unaligned_beacon_dx == -aligned_delta);
              return &Point3D::x_;
            }
            else if (abs(unaligned_beacon_dy) == abs(aligned_delta)) {
              negate = (unaligned_beacon_dy == -aligned_delta);
              return &Point3D::y_;
            }
            else if (abs(unaligned_beacon_dz) == abs(aligned_delta)) {
              negate = (unaligned_beacon_dz == -aligned_delta);
              return &Point3D::z_;
            }
            else {
              assert(false);
              return &Point3D::x_;
            }
          };
          bool negate_x = false, negate_y = false, negate_z = false;
          auto x_source = alignment_func(negate_x, aligned_beacon_dx);
          auto y_source = alignment_func(negate_y, aligned_beacon_dy);
          auto z_source = alignment_func(negate_z, aligned_beacon_dz);
          
          // Update all of the beacons to have the correct alignment
          for (auto & this_unaligned_beacon : unaligned_scanner.beacons_) {
            Point3D old_location = this_unaligned_beacon.location_;
            this_unaligned_beacon.location_.x_ = (old_location.*x_source) * (negate_x ? -1 : 1);
            this_unaligned_beacon.location_.y_ = (old_location.*y_source) * (negate_y ? -1 : 1);
            this_unaligned_beacon.location_.z_ = (old_location.*z_source) * (negate_z ? -1 : 1);
          }
          
          // Given the current match, now with the correct alignment, we can determine the
          // scanner's position relative to the "home" scanner.
          unaligned_scanner.position_ = Point3D(aligned_beacon.location_.x_ - unaligned_beacon.location_.x_,
                                                aligned_beacon.location_.y_ - unaligned_beacon.location_.y_,
                                                aligned_beacon.location_.z_ - unaligned_beacon.location_.z_);
          
          // Finally, update all of the unaligned beacons to be relative to the "home"
          // scanner. Note that this makes the current (previously unaligned) scanner
          // superfluous (its beacons' locations are now relative to a different
          // scanner), but we're going to hold onto it for part two.
          for (auto & unpositioned_beacon : unaligned_scanner.beacons_) {
            unpositioned_beacon.location_.x_ += unaligned_scanner.position_.x_;
            unpositioned_beacon.location_.y_ += unaligned_scanner.position_.y_;
            unpositioned_beacon.location_.z_ += unaligned_scanner.position_.z_;
          }
          
          return true;
        }
      }
    }
    return false;
  }
}

int main()
{
  std::ifstream input("AoC_19_input.dat");
  
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  std::list<Scanner> unaligned;
  process_file(input, unaligned);
  
  // Assume the first scanner is "home" and align the other scanners to match.
  std::list<Scanner> aligned;
  aligned.splice(aligned.end(), unaligned, unaligned.begin());
  while (!unaligned.empty()) {
    bool found = false;
    for (auto unaligned_iter = unaligned.begin(); !found && unaligned_iter != unaligned.end(); ++unaligned_iter) {
      for (auto aligned_iter = aligned.begin(); !found && aligned_iter != aligned.end(); ++aligned_iter) {
        found = align_scanners(*aligned_iter, *unaligned_iter);
        if (found) {
          aligned.splice(aligned.end(), unaligned, unaligned_iter);
        }
      }
    }
  }
  
  // Count the total number of beacons by finding the number of unique
  // locations across all scanners.
  std::set<Point3D> beacon_count;
  for (auto scanner : aligned) {
    for (auto beacon : scanner.beacons_) {
      beacon_count.insert(beacon.location_);
    }
  }
  std::cout << "Part One: " << beacon_count.size() << "\n";
  
  int max = 0;
  for (auto back_iter = aligned.begin(); back_iter != aligned.end(); ++back_iter) {
    auto front_iter = back_iter;
    ++front_iter;
    while (front_iter != aligned.end()) {
      max = std::max(max, get_distance(front_iter++->position_, back_iter->position_));
    }
  }
  std::cout << "Part Two: " << max << "\n";
  
  return 0;
}
