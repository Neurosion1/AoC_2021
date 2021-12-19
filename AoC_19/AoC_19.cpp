#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include <sstream>
#include <map>

namespace
{
  struct Point3D
  {
    Point3D(int x, int y, int z) : x_(x), y_(y), z_(z) {}
    
    int x_, y_, z_;
  };

  struct Sensor
  {
    Sensor(std::vector<Point3D> beacons) : beacons_(beacons) {}
    
    std::vector<Point3D> beacons_;
  };

  void process_file(std::ifstream& input, std::vector<Sensor>& sensors)
  {
    do {
      char buffer[5000];
      input.getline(buffer, 5000);
      std::vector<Point3D> beacons;
      do {
        input.getline(buffer, 5000);
        if (strlen(buffer) == 0) {
          break;
        }
        std::stringstream str(buffer);
        int x, y, z;
        char comma;
        str >> x >> comma >> y >> comma >> z;
        beacons.push_back(Point3D(x, y, z));
      } while (true);
      sensors.push_back(beacons);
    } while (!input.eof());
  }

  float get_distance(Point3D p1, Point3D p2)
  {
    return sqrt((p2.x_ - p1.x_) * (p2.x_ - p1.x_) +
                (p2.y_ - p1.y_) * (p2.y_ - p1.y_) +
                (p2.z_ - p1.z_) * (p2.z_ - p1.z_));
  }
}

int main()
{
  std::ifstream input("AoC_19_input.dat");
  
  if (input.fail()) {
    std::cout << "Didn't find the input file\n";
    return 1;
  }
  std::vector<Sensor> sensors;
  process_file(input, sensors);
  
  std::cout << sensors.size() << "\n";
  
  std::multimap<float, int> distances;
  for (int s = 0; s < sensors.size(); ++s) {
    auto & beacons = sensors[s].beacons_;
    for (int b1 = 0; b1 < beacons.size() - 1; ++b1) {
      for (int b2 = b1 + 1; b2 < beacons.size(); ++b2) {
        distances.insert( { get_distance(beacons[b1], beacons[b2]), s } );
      }
    }
  }
  
  std::vector<float> detected;
  for (auto iter = distances.begin(); iter != distances.end();) {
    if (distances.count(iter->first) >= 3 && iter->second == 0) {
      detected.push_back(iter->first);
    }
    iter = distances.upper_bound(iter->first);
  }
  
  //std::cout << "Part One: " << bitsMachine.version_sum_ << "\n";
  //std::cout << "Part Two: " << result << std::endl;
  // 84?
  return 0;
}

